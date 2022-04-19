#include "z_en_dodongo.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Bombf/z_en_bombf.h"
#include "objects/object_dodongo/object_dodongo.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

typedef enum {
    DODONGO_SWEEP_TAIL,
    DODONGO_SWALLOW_BOMB,
    DODONGO_DEATH,
    DODONGO_BREATHE_FIRE,
    DODONGO_IDLE,
    DODONGO_END_BREATHE_FIRE,
    DODONGO_UNUSED,
    DODONGO_STUNNED,
    DODONGO_WALK
} EnDodongoActionState;

void EnDodongo_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDodongo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDodongo_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDodongo_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDodongo_SetupDeath(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_ShiftVecRadial(s16 yaw, f32 radius, Vec3f* vec);
s32 EnDodongo_AteBomb(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_SetupIdle(EnDodongo* thisv);

void EnDodongo_Idle(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_EndBreatheFire(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_BreatheFire(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_SwallowBomb(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_Walk(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_Stunned(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_Death(EnDodongo* thisv, GlobalContext* globalCtx);
void EnDodongo_SweepTail(EnDodongo* thisv, GlobalContext* globalCtx);

ActorInit En_Dodongo_InitVars = {
    ACTOR_EN_DODONGO,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DODONGO,
    sizeof(EnDodongo),
    (ActorFunc)EnDodongo_Init,
    (ActorFunc)EnDodongo_Destroy,
    (ActorFunc)EnDodongo_Update,
    (ActorFunc)EnDodongo_Draw,
    NULL,
};

static ColliderJntSphElementInit sBodyElementsInit[6] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 15, { { 0, 0, 0 }, 17 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 14, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 21, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 28, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0D800691, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON | OCELEM_UNK3,
        },
        { 6, { { 0, 0, 0 }, 35 }, 100 },
    },
};

static ColliderJntSphInit sBodyJntSphInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    6,
    sBodyElementsInit,
};

static ColliderTrisElementInit sHardElementsInit[3] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xF24BF96E, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCBF96E, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCBF96E, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit sHardTrisInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    3,
    sHardElementsInit,
};

static ColliderQuadInit sAttackQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x01, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(1, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(4, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0xF),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
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

void EnDodongo_SetupAction(EnDodongo* thisv, EnDodongoActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnDodongo_SpawnBombSmoke(EnDodongo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos = thisv->headPos;
    s16 randAngle = Rand_CenteredFloat(0x4000);
    f32 randCos;
    f32 randSin;

    randCos = Math_CosS(thisv->actor.shape.rot.y + randAngle);
    randSin = Math_SinS(thisv->actor.shape.rot.y + randAngle);
    if (thisv->bombSmokePrimColor.r > 30) {
        thisv->bombSmokePrimColor.r -= 16;
        thisv->bombSmokePrimColor.g -= 16;
    }

    if (thisv->bombSmokePrimColor.b < 30) {
        thisv->bombSmokePrimColor.b += 5;
        thisv->bombSmokePrimColor.a += 8;
        thisv->bombSmokeEnvColor.a += 8;
    }
    if (thisv->bombSmokeEnvColor.r != 0) {
        thisv->bombSmokeEnvColor.r -= 15;
    }
    if (thisv->bombSmokeEnvColor.g != 0) {
        thisv->bombSmokeEnvColor.g--;
    }
    velocity.x = randSin * 3.5f;
    velocity.y = thisv->bombSmokeEnvColor.r * 0.02f;
    velocity.z = randCos * 3.5f;
    accel.x = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randSin;
    accel.z = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randCos;
    func_8002836C(globalCtx, &pos, &velocity, &accel, &thisv->bombSmokePrimColor, &thisv->bombSmokeEnvColor, 100, 25, 20);

    randAngle = Rand_ZeroOne() * 0x2000;
    randCos = Math_CosS(thisv->actor.shape.rot.y + randAngle);
    randSin = Math_SinS(thisv->actor.shape.rot.y + randAngle);
    pos.x -= randCos * 6.0f;
    pos.z += randSin * 6.0f;
    velocity.x = -randCos * 3.5f;
    velocity.y = thisv->bombSmokeEnvColor.r * 0.02f;
    velocity.z = randSin * 3.5f;
    accel.x = ((Rand_ZeroOne() * 0.1f) + 0.15f) * randCos;
    accel.z = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randSin;
    func_8002836C(globalCtx, &pos, &velocity, &accel, &thisv->bombSmokePrimColor, &thisv->bombSmokeEnvColor, 100, 25, 20);

    randAngle = Rand_ZeroOne() * 0x2000;
    randCos = Math_CosS(thisv->actor.shape.rot.y + randAngle);
    randSin = Math_SinS(thisv->actor.shape.rot.y + randAngle);

    pos.x = thisv->headPos.x + (randCos * 6.0f);
    pos.z = thisv->headPos.z - (randSin * 6.0f);
    velocity.x = randCos * 3.5f;
    velocity.y = thisv->bombSmokeEnvColor.r * 0.02f;
    velocity.z = -randSin * 3.5f;
    accel.x = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randCos;
    accel.z = ((Rand_ZeroOne() * 0.1f) + 0.15f) * randSin;
    func_8002836C(globalCtx, &pos, &velocity, &accel, &thisv->bombSmokePrimColor, &thisv->bombSmokeEnvColor, 100, 25, 20);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x0D, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2800, ICHAIN_STOP),
};

void EnDodongo_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDodongo* thisv = (EnDodongo*)thisx;
    EffectBlureInit1 blureInit;

    thisv->actor.targetMode = 3;
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->bombSmokePrimColor.r = thisv->bombSmokePrimColor.g = thisv->bombSmokeEnvColor.r = 255;
    thisv->bombSmokePrimColor.a = thisv->bombSmokeEnvColor.a = 200;
    thisv->bombSmokeEnvColor.g = 10;
    thisv->bodyScale.x = thisv->bodyScale.y = thisv->bodyScale.z = 1.0f;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
    Actor_SetScale(&thisv->actor, 0.01875f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gDodongoSkel, &gDodongoWaitAnim, thisv->jointTable, thisv->morphTable,
                   31);
    thisv->actor.colChkInfo.health = 4;
    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    Collider_InitQuad(globalCtx, &thisv->colliderAT);
    Collider_InitTris(globalCtx, &thisv->colliderHard);
    Collider_InitJntSph(globalCtx, &thisv->colliderBody);
    Collider_SetQuad(globalCtx, &thisv->colliderAT, &thisv->actor, &sAttackQuadInit);
    Collider_SetTris(globalCtx, &thisv->colliderHard, &thisv->actor, &sHardTrisInit, thisv->trisElements);
    Collider_SetJntSph(globalCtx, &thisv->colliderBody, &thisv->actor, &sBodyJntSphInit, thisv->sphElements);

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;

    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.p2StartColor[3] = 64;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = false;
    blureInit.calcMode = 2;

    Effect_Add(globalCtx, &thisv->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 60.0f, 70.0f, 0x1D);
    EnDodongo_SetupIdle(thisv);
}

void EnDodongo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDodongo* thisv = (EnDodongo*)thisx;

    Effect_Delete(globalCtx, thisv->blureIdx);
    Collider_DestroyTris(globalCtx, &thisv->colliderHard);
    Collider_DestroyJntSph(globalCtx, &thisv->colliderBody);
    Collider_DestroyQuad(globalCtx, &thisv->colliderAT);
}

void EnDodongo_SetupIdle(EnDodongo* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gDodongoWaitAnim, -4.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->timer = Rand_S16Offset(30, 50);
    thisv->actionState = DODONGO_IDLE;
    EnDodongo_SetupAction(thisv, EnDodongo_Idle);
}

void EnDodongo_SetupWalk(EnDodongo* thisv) {
    f32 frames = Animation_GetLastFrame(&gDodongoWalkAnim);

    Animation_Change(&thisv->skelAnime, &gDodongoWalkAnim, 0.0f, 0.0f, frames, ANIMMODE_LOOP, -4.0f);
    thisv->actor.speedXZ = 1.5f;
    thisv->timer = Rand_S16Offset(50, 70);
    thisv->rightFootStep = true;
    thisv->actionState = DODONGO_WALK;
    EnDodongo_SetupAction(thisv, EnDodongo_Walk);
}

void EnDodongo_SetupBreatheFire(EnDodongo* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gDodongoBreatheFireAnim, -4.0f);
    thisv->actionState = DODONGO_BREATHE_FIRE;
    thisv->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(thisv, EnDodongo_BreatheFire);
}

void EnDodongo_SetupEndBreatheFire(EnDodongo* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gDodongoAfterBreatheFireAnim);
    thisv->actionState = DODONGO_END_BREATHE_FIRE;
    thisv->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(thisv, EnDodongo_EndBreatheFire);
}

void EnDodongo_SetupSwallowBomb(EnDodongo* thisv) {
    Animation_Change(&thisv->skelAnime, &gDodongoBreatheFireAnim, -1.0f, 35.0f, 0.0f, ANIMMODE_ONCE, -4.0f);
    thisv->actionState = DODONGO_SWALLOW_BOMB;
    thisv->timer = 25;
    thisv->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(thisv, EnDodongo_SwallowBomb);
}

void EnDodongo_SetupStunned(EnDodongo* thisv) {
    Animation_Change(&thisv->skelAnime, &gDodongoBreatheFireAnim, 0.0f, 25.0f, 0.0f, ANIMMODE_ONCE, -4.0f);
    thisv->actionState = DODONGO_STUNNED;
    thisv->actor.speedXZ = 0.0f;
    if (thisv->damageEffect == 0xF) {
        thisv->iceTimer = 36;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnDodongo_SetupAction(thisv, EnDodongo_Stunned);
}

void EnDodongo_Idle(EnDodongo* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((DECR(thisv->timer) == 0) && Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        EnDodongo_SetupWalk(thisv);
    }
}

void EnDodongo_EndBreatheFire(EnDodongo* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnDodongo_SetupIdle(thisv);
        thisv->timer = Rand_S16Offset(10, 20);
    }
}

void EnDodongo_BreatheFire(EnDodongo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    s16 pad2;
    s16 fireFrame;

    if ((s32)thisv->skelAnime.curFrame == 24) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_CRY);
    }
    if ((29.0f <= thisv->skelAnime.curFrame) && (thisv->skelAnime.curFrame <= 43.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_FIRE - SFX_FLAG);
        fireFrame = thisv->skelAnime.curFrame - 29.0f;
        pos = thisv->actor.world.pos;
        pos.y += 35.0f;
        EnDodongo_ShiftVecRadial(thisv->actor.world.rot.y, 30.0f, &pos);
        EnDodongo_ShiftVecRadial(thisv->actor.world.rot.y, 2.5f, &accel);
        EffectSsDFire_SpawnFixedScale(globalCtx, &pos, &velocity, &accel, 255 - (fireFrame * 10), fireFrame + 3);
    } else if ((2.0f <= thisv->skelAnime.curFrame) && (thisv->skelAnime.curFrame <= 20.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_BREATH - SFX_FLAG);
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnDodongo_SetupEndBreatheFire(thisv);
    }
}

void EnDodongo_SwallowBomb(EnDodongo* thisv, GlobalContext* globalCtx) {
    Vec3f smokeVel = { 0.0f, 0.0f, 0.0f };
    Vec3f smokeAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 white = { 255, 255, 255, 255 };
    Vec3f deathFireVel = { 0.0f, 0.0f, 0.0f };
    Vec3f deathFireAccel = { 0.0f, 1.0f, 0.0f };
    s16 i;
    Vec3f pos;
    s32 pad;

    if (thisv->actor.child != NULL) {
        thisv->actor.child->world.pos = thisv->mouthPos;
        ((EnBom*)thisv->actor.child)->timer++;
    } else if (thisv->actor.parent != NULL) {
        thisv->actor.parent->world.pos = thisv->mouthPos;
        ((EnBombf*)thisv->actor.parent)->timer++;
        //! @bug An explosive can also be a bombchu, not always a bomb, which leads to a serious bug. ->timer (0x1F8) is
        //! outside the bounds of the bombchu actor, and the memory it writes to happens to be one of the pointers in
        //! the next arena node. When thisv value is written to, massive memory corruption occurs.
    }

    if ((s32)thisv->skelAnime.curFrame == 28) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_EAT);
        if (thisv->actor.child != NULL) {
            Actor_Kill(thisv->actor.child);
            thisv->actor.child = NULL;
        } else if (thisv->actor.parent != NULL) {
            Actor_Kill(thisv->actor.parent);
            thisv->actor.parent = NULL;
        }
    } else if ((s32)thisv->skelAnime.curFrame == 24) {
        thisv->timer--;
        if (thisv->timer != 0) {
            thisv->skelAnime.curFrame++;
            if (thisv->timer == 10) {
                for (i = 10; i >= 0; i--) {
                    deathFireVel.x = Rand_CenteredFloat(10.0f);
                    deathFireVel.y = Rand_CenteredFloat(10.0f);
                    deathFireVel.z = Rand_CenteredFloat(10.0f);
                    deathFireAccel.x = deathFireVel.x * -0.1f;
                    deathFireAccel.y = deathFireVel.y * -0.1f;
                    deathFireAccel.z = deathFireVel.z * -0.1f;
                    pos.x = thisv->sphElements[0].dim.worldSphere.center.x + deathFireVel.x;
                    pos.y = thisv->sphElements[0].dim.worldSphere.center.y + deathFireVel.y;
                    pos.z = thisv->sphElements[0].dim.worldSphere.center.z + deathFireVel.z;
                    func_8002836C(globalCtx, &pos, &deathFireVel, &deathFireAccel, &thisv->bombSmokePrimColor,
                                  &thisv->bombSmokeEnvColor, 400, 10, 10);
                }
                Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_BOMB_EXPLOSION);
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0x78, 0, 8);
            }
        }
    }
    if ((s32)thisv->skelAnime.curFrame < 28) {
        if (((s32)thisv->skelAnime.curFrame < 26) && (thisv->timer <= 10)) {
            EnDodongo_SpawnBombSmoke(thisv, globalCtx);
        } else {
            pos = thisv->headPos;
            func_8002829C(globalCtx, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
            pos.x -= (Math_CosS(thisv->actor.shape.rot.y) * 6.0f);
            pos.z += (Math_SinS(thisv->actor.shape.rot.y) * 6.0f);
            func_8002829C(globalCtx, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
            pos.x = thisv->headPos.x + (Math_CosS(thisv->actor.shape.rot.y) * 6.0f);
            pos.z = thisv->headPos.z - (Math_SinS(thisv->actor.shape.rot.y) * 6.0f);
            func_8002829C(globalCtx, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
        }
    }
    thisv->bodyScale.y = thisv->bodyScale.z = (Math_SinS(thisv->actor.colorFilterTimer * 0x1000) * 0.5f) + 1.0f;
    thisv->bodyScale.x = Math_SinS(thisv->actor.colorFilterTimer * 0x1000) + 1.0f;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer == 0) {
        EnDodongo_SetupDeath(thisv, globalCtx);
    }
}

void EnDodongo_Walk(EnDodongo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 playbackSpeed;
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff = (s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y);

    yawDiff = ABS(yawDiff);

    Math_SmoothStepToF(&thisv->actor.speedXZ, 1.5f, 0.1f, 1.0f, 0.0f);

    playbackSpeed = thisv->actor.speedXZ * 0.75f;
    if (thisv->actor.speedXZ >= 0.0f) {
        if (playbackSpeed > 3.0f / 2) {
            playbackSpeed = 3.0f / 2;
        }
    } else {
        if (playbackSpeed < -3.0f / 2) {
            playbackSpeed = -3.0f / 2;
        }
    }
    thisv->skelAnime.playSpeed = playbackSpeed;

    SkelAnime_Update(&thisv->skelAnime);
    if ((s32)thisv->skelAnime.curFrame < 21) {
        if (!thisv->rightFootStep) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_WALK);
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->leftFootPos, 10.0f, 3, 2.0f, 0xC8, 0xF, 0);
            thisv->rightFootStep = true;
        }
    } else {
        if (thisv->rightFootStep) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_WALK);
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->rightFootPos, 10.0f, 3, 2.0f, 0xC8, 0xF, 0);
            thisv->rightFootStep = false;
        }
    }

    if (Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < 400.0f) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x1F4, 0);
        thisv->actor.flags |= ACTOR_FLAG_0;
        if ((thisv->actor.xzDistToPlayer < 100.0f) && (yawDiff < 0x1388) && (thisv->actor.yDistToPlayer < 60.0f)) {
            EnDodongo_SetupBreatheFire(thisv);
        }
    } else {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        if ((Math_Vec3f_DistXZ(&thisv->actor.world.pos, &thisv->actor.home.pos) > 150.0f) || (thisv->retreatTimer != 0)) {
            s16 yawToHome = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);

            Math_SmoothStepToS(&thisv->actor.world.rot.y, yawToHome, 1, 0x1F4, 0);
        }
        if (thisv->retreatTimer != 0) {
            thisv->retreatTimer--;
        }
        thisv->timer--;
        if (thisv->timer == 0) {
            if (Rand_ZeroOne() > 0.7f) {
                thisv->timer = Rand_S16Offset(50, 70);
                thisv->retreatTimer = Rand_S16Offset(15, 40);
            } else {
                EnDodongo_SetupIdle(thisv);
            }
        }
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnDodongo_SetupSweepTail(EnDodongo* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gDodongoDamageAnim, -4.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_DAMAGE);
    thisv->actionState = DODONGO_SWEEP_TAIL;
    thisv->timer = 0;
    thisv->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(thisv, EnDodongo_SweepTail);
}

void EnDodongo_SweepTail(EnDodongo* thisv, GlobalContext* globalCtx) {
    s16 yawDiff1 = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if ((thisv->timer != 0) || (ABS(yawDiff1) < 0x4000)) {
            thisv->sphElements[2].info.toucherFlags = TOUCH_NONE;
            thisv->sphElements[1].info.toucherFlags = TOUCH_NONE;
            thisv->colliderBody.base.atFlags = AT_NONE;
            thisv->sphElements[2].info.toucher.dmgFlags = 0;
            thisv->sphElements[1].info.toucher.dmgFlags = 0;
            thisv->sphElements[2].info.toucher.damage = 0;
            thisv->sphElements[1].info.toucher.damage = 0;
            EnDodongo_SetupBreatheFire(thisv);
            thisv->timer = Rand_S16Offset(5, 10);
        } else {
            s16 yawDiff2 = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
            const AnimationHeader* animation;

            thisv->tailSwipeSpeed = (0xFFFF - ABS(yawDiff2)) / 0xF;
            if ((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y) >= 0) {
                thisv->tailSwipeSpeed = -thisv->tailSwipeSpeed;
                animation = &gDodongoSweepTailLeftAnim;
            } else {
                animation = &gDodongoSweepTailRightAnim;
            }
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_TAIL);
            Animation_PlayOnceSetSpeed(&thisv->skelAnime, animation, 2.0f);
            thisv->timer = 18;
            thisv->colliderBody.base.atFlags = thisv->sphElements[1].info.toucherFlags =
                thisv->sphElements[2].info.toucherFlags = AT_ON | AT_TYPE_ENEMY; // also TOUCH_ON | TOUCH_SFX_WOOD
            thisv->sphElements[1].info.toucher.dmgFlags = thisv->sphElements[2].info.toucher.dmgFlags = 0xFFCFFFFF;
            thisv->sphElements[1].info.toucher.damage = thisv->sphElements[2].info.toucher.damage = 8;
        }
    } else if (thisv->timer > 1) {
        Vec3f tailPos;

        thisv->timer--;
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y += thisv->tailSwipeSpeed;
        tailPos.x = thisv->sphElements[1].dim.worldSphere.center.x;
        tailPos.y = thisv->sphElements[1].dim.worldSphere.center.y;
        tailPos.z = thisv->sphElements[1].dim.worldSphere.center.z;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &tailPos, 5.0f, 2, 2.0f, 100, 15, 0);
        tailPos.x = thisv->sphElements[2].dim.worldSphere.center.x;
        tailPos.y = thisv->sphElements[2].dim.worldSphere.center.y;
        tailPos.z = thisv->sphElements[2].dim.worldSphere.center.z;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &tailPos, 5.0f, 2, 2.0f, 100, 15, 0);

        if (thisv->colliderBody.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(globalCtx);

            if (thisv->colliderBody.base.at == &player->actor) {
                Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
            }
        }
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
    }
}

void EnDodongo_SetupDeath(EnDodongo* thisv, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gDodongoDieAnim, -8.0f);
    thisv->timer = 0;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_J_DEAD);
    thisv->actionState = DODONGO_DEATH;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(thisv, EnDodongo_Death);
}

void EnDodongo_Death(EnDodongo* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;

    if (thisv->skelAnime.curFrame < 35.0f) {
        if (thisv->actor.params == EN_DODONGO_SMOKE_DEATH) {
            EnDodongo_SpawnBombSmoke(thisv, globalCtx);
        }
    } else if (thisv->actor.colorFilterTimer == 0) {
        Actor_SetColorFilter(&thisv->actor, 0x4000, 0x78, 0, 4);
    }
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        if (thisv->timer == 0) {
            bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->actor.world.pos.x,
                                       thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 6, BOMB_BODY);
            if (bomb != NULL) {
                bomb->timer = 0;
                thisv->timer = 8;
            }
        }
    } else if ((s32)thisv->skelAnime.curFrame == 52) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
    }
    if (thisv->timer != 0) {
        thisv->timer--;
        if (thisv->timer == 0) {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x40);
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnDodongo_Stunned(EnDodongo* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.colorFilterTimer == 0) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnDodongo_SetupDeath(thisv, globalCtx);
        } else {
            EnDodongo_SetupIdle(thisv);
        }
    }
}

void EnDodongo_CollisionCheck(EnDodongo* thisv, GlobalContext* globalCtx) {
    if (thisv->colliderHard.base.acFlags & AC_BOUNCED) {
        thisv->colliderHard.base.acFlags &= ~AC_BOUNCED;
        thisv->colliderBody.base.acFlags &= ~AC_HIT;
    } else if ((thisv->colliderBody.base.acFlags & AC_HIT) && (thisv->actionState > DODONGO_DEATH)) {
        thisv->colliderBody.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&thisv->actor, &thisv->colliderBody, 0);
        if (thisv->actor.colChkInfo.damageEffect != 0xE) {
            thisv->damageEffect = thisv->actor.colChkInfo.damageEffect;
            if ((thisv->actor.colChkInfo.damageEffect == 1) || (thisv->actor.colChkInfo.damageEffect == 0xF)) {
                if (thisv->actionState != DODONGO_STUNNED) {
                    Actor_SetColorFilter(&thisv->actor, 0, 0x78, 0, 0x50);
                    Actor_ApplyDamage(&thisv->actor);
                    EnDodongo_SetupStunned(thisv);
                }
            } else {
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0x78, 0, 8);
                if (Actor_ApplyDamage(&thisv->actor) == 0) {
                    EnDodongo_SetupDeath(thisv, globalCtx);
                } else {
                    EnDodongo_SetupSweepTail(thisv);
                }
            }
        }
    }
}

void EnDodongo_UpdateQuad(EnDodongo* thisv, GlobalContext* globalCtx) {
    Vec3f sp94 = { -1000.0f, -1500.0f, 0.0f };
    Vec3f sp88 = { -1000.0f, -200.0f, 1500.0f };
    Vec3f sp7C = { -1000.0f, -200.0f, -1500.0f };
    Vec3f sp70 = { 0.0f, 0.0f, 0.0f };
    s32 pad4C[9]; // Possibly 3 more Vec3fs?
    s32 a = 0;
    s32 b = 1; // These indices are needed to match.
    s32 c = 2; // Might be a way to quickly test vertex arrangements
    s32 d = 3;
    f32 xMod = Math_SinF((thisv->skelAnime.curFrame - 28.0f) * 0.08f) * 5500.0f;

    sp7C.x -= xMod;
    sp94.x -= xMod;
    sp88.x -= xMod;

    Matrix_MultVec3f(&sp94, &thisv->colliderAT.dim.quad[b]);
    Matrix_MultVec3f(&sp88, &thisv->colliderAT.dim.quad[a]);
    Matrix_MultVec3f(&sp7C, &thisv->colliderAT.dim.quad[d]);
    Matrix_MultVec3f(&sp70, &thisv->colliderAT.dim.quad[c]);

    Collider_SetQuadVertices(&thisv->colliderAT, &thisv->colliderAT.dim.quad[a], &thisv->colliderAT.dim.quad[b],
                             &thisv->colliderAT.dim.quad[c], &thisv->colliderAT.dim.quad[d]);
}

void EnDodongo_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDodongo* thisv = (EnDodongo*)thisx;

    EnDodongo_CollisionCheck(thisv, globalCtx);
    if (thisv->actor.colChkInfo.damageEffect != 0xE) {
        thisv->actionFunc(thisv, globalCtx);
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 60.0f, 70.0f, 0x1D);
        if (thisv->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
        }
    }
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
    if (thisv->actionState != DODONGO_DEATH) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderHard.base);
    }
    if (thisv->actionState > DODONGO_DEATH) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
    }
    if ((thisv->actionState >= DODONGO_IDLE) && EnDodongo_AteBomb(thisv, globalCtx)) {
        EnDodongo_SetupSwallowBomb(thisv);
    }
    if (thisv->actionState == DODONGO_BREATHE_FIRE) {
        if ((29.0f < thisv->skelAnime.curFrame) && (thisv->skelAnime.curFrame < 43.0f)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderAT.base);
        }
    }
    thisv->actor.focus.pos.x = thisv->actor.world.pos.x + Math_SinS(thisv->actor.shape.rot.y) * -30.0f;
    thisv->actor.focus.pos.y = thisv->actor.world.pos.y + 20.0f;
    thisv->actor.focus.pos.z = thisv->actor.world.pos.z + Math_CosS(thisv->actor.shape.rot.y) * -30.0f;
}

s32 EnDodongo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    EnDodongo* thisv = (EnDodongo*)thisx;

    if ((limbIndex == 15) || (limbIndex == 16)) {
        Matrix_Scale(thisv->bodyScale.x, thisv->bodyScale.y, thisv->bodyScale.z, MTXMODE_APPLY);
    }
    return false;
}

void EnDodongo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f legOffsets[3] = {
        { 1100.0f, -700.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 2190.0f, 0.0f, 0.0f },
    };
    Vec3f tailTipOffset = { 3000.0f, 0.0f, 0.0f };
    Vec3f baseOffset = { 0.0f, 0.0f, 0.0f };
    s32 i;
    Vec3f hardTris0Vtx[3];
    Vec3f hardTris1Vtx[3];
    Vec3f hardTris2Vtx[3];
    Vec3f tailTip;
    Vec3f tailBase;
    EnDodongo* thisv = (EnDodongo*)thisx;
    Vec3f hardTris0VtxOffset[] = {
        { -300.0f, -2500.0f, 0.0f },
        { -300.0f, 1200.0f, -2700.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f hardTris1VtxOffset[] = {
        { -300.0f, -2500.0f, 0.0f },
        { -300.0f, 1200.0f, 2700.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f hardTris2VtxOffset[] = {
        { -600.0f, 1200.0f, -2800.0f },
        { -600.0f, 1200.0f, 2800.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f mouthOffset = { 1800.0f, 1200.0f, 0.0f };
    Vec3f headOffset = { 1500.0f, 300.0f, 0.0f };

    Collider_UpdateSpheres(limbIndex, &thisv->colliderBody);

    switch (limbIndex) {
        case 2:
            if ((thisv->actionState == DODONGO_BREATHE_FIRE) && (29.0f < thisv->skelAnime.curFrame) &&
                (thisv->skelAnime.curFrame < 43.0f)) {
                EnDodongo_UpdateQuad(thisv, globalCtx);
            }
            break;
        case 7:
            for (i = 0; i < 3; i++) {
                Matrix_MultVec3f(&hardTris0VtxOffset[i], &hardTris0Vtx[i]);
                Matrix_MultVec3f(&hardTris1VtxOffset[i], &hardTris1Vtx[i]);
                Matrix_MultVec3f(&hardTris2VtxOffset[i], &hardTris2Vtx[i]);
            }
            Collider_SetTrisVertices(&thisv->colliderHard, 0, &hardTris0Vtx[0], &hardTris0Vtx[1], &hardTris0Vtx[2]);
            Collider_SetTrisVertices(&thisv->colliderHard, 1, &hardTris1Vtx[0], &hardTris1Vtx[1], &hardTris1Vtx[2]);
            Collider_SetTrisVertices(&thisv->colliderHard, 2, &hardTris2Vtx[0], &hardTris2Vtx[1], &hardTris2Vtx[2]);
            Matrix_MultVec3f(&mouthOffset, &thisv->mouthPos);
            Matrix_MultVec3f(&headOffset, &thisv->headPos);
            break;
        case 15:
            if ((thisv->actionState == DODONGO_SWEEP_TAIL) && (thisv->timer >= 2)) {
                Matrix_MultVec3f(&tailTipOffset, &tailTip);
                Matrix_MultVec3f(&baseOffset, &tailBase);
                EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blureIdx)), &tailTip, &tailBase);
            } else if ((thisv->actionState == DODONGO_SWEEP_TAIL) && (thisv->timer != 0)) {
                EffectBlure_AddSpace(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blureIdx)));
            }
            break;
        case 21:
            Matrix_MultVec3f(&legOffsets[1], &thisv->leftFootPos);
            break;
        case 28:
            Matrix_MultVec3f(&legOffsets[1], &thisv->rightFootPos);
            break;
    }
    if (thisv->iceTimer != 0) {
        i = -1;
        switch (limbIndex) {
            case 7:
                baseOffset.x = 1200.0f;
                i = 0;
                break;
            case 13:
                i = 1;
                break;
            case 14:
                i = 2;
                break;
            case 15:
                i = 3;
                break;
            case 16:
                i = 4;
                break;
            case 22:
                i = 5;
                break;
            case 29:
                i = 6;
                break;
            case 21:
                i = 7;
                break;
            case 28:
                i = 8;
                break;
        }
        if (i >= 0) {
            Matrix_MultVec3f(&baseOffset, &thisv->icePos[i]);
        }
    }
}

void EnDodongo_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnDodongo* thisv = (EnDodongo*)thisx;
    s32 index;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnDodongo_OverrideLimbDraw,
                      EnDodongo_PostLimbDraw, thisv);

    if (thisv->iceTimer != 0) {
        thisv->actor.colorFilterTimer++;
        if (1) {}
        thisv->iceTimer--;
        if ((thisv->iceTimer % 4) == 0) {
            index = thisv->iceTimer >> 2;
            EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &thisv->icePos[index], 150, 150, 150, 250, 235, 245,
                                           255, 1.8f);
        }
    }
}

void EnDodongo_ShiftVecRadial(s16 yaw, f32 radius, Vec3f* vec) {
    vec->x += Math_SinS(yaw) * radius;
    vec->z += Math_CosS(yaw) * radius;
}

s32 EnDodongo_AteBomb(EnDodongo* thisv, GlobalContext* globalCtx) {
    Actor* actor = globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    f32 dx;
    f32 dy;
    f32 dz;

    while (actor != NULL) {
        if ((actor->params != 0) || (actor->parent != NULL)) {
            actor = actor->next;
            continue;
        }
        dx = actor->world.pos.x - thisv->mouthPos.x;
        dy = actor->world.pos.y - thisv->mouthPos.y;
        dz = actor->world.pos.z - thisv->mouthPos.z;
        if ((fabsf(dx) < 20.0f) && (fabsf(dy) < 10.0f) && (fabsf(dz) < 20.0f)) {
            if (actor->id == ACTOR_EN_BOM) {
                thisv->actor.child = actor;
            } else {
                thisv->actor.parent = actor;
            }
            actor->parent = &thisv->actor;
            return true;
        }
        actor = actor->next;
    }
    return false;
}
