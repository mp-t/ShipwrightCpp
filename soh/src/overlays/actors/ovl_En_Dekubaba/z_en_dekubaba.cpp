#include "z_en_dekubaba.h"
#include "objects/object_dekubaba/object_dekubaba.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnDekubaba_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDekubaba_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDekubaba_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDekubaba_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnDekuBaba_Reset(void);

void EnDekubaba_SetupWait(EnDekubaba* thisv);
void EnDekubaba_SetupGrow(EnDekubaba* thisv);
void EnDekubaba_Wait(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_Grow(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_Retract(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_DecideLunge(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_Lunge(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_PrepareLunge(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_PullBack(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_Recover(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_Hit(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_StunnedVertical(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_Sway(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_PrunedSomersault(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_ShrinkDie(EnDekubaba* thisv, GlobalContext* globalCtx);
void EnDekubaba_DeadStickDrop(EnDekubaba* thisv, GlobalContext* globalCtx);

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

ActorInit En_Dekubaba_InitVars = {
    ACTOR_EN_DEKUBABA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DEKUBABA,
    sizeof(EnDekubaba),
    (ActorFunc)EnDekubaba_Init,
    (ActorFunc)EnDekubaba_Destroy,
    (ActorFunc)EnDekubaba_Update,
    (ActorFunc)EnDekubaba_Draw,
    (ActorResetFunc)EnDekuBaba_Reset,
};

static ColliderJntSphElementInit sJntSphElementsInit[7] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_HARD,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 100, 1000 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 51, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 52, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 53, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 54, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 55, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 56, { { 0, 0, 500 }, 8 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    7,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0x0 */ DEKUBABA_DMGEFF_NONE,
    /* 0x1 */ DEKUBABA_DMGEFF_DEKUNUT,
    /* 0x2 */ DEKUBABA_DMGEFF_FIRE,
    /* 0xE */ DEKUBABA_DMGEFF_BOOMERANG = 14,
    /* 0xF */ DEKUBABA_DMGEFF_SWORD
} DekuBabaDamageEffect;

static DamageTable sDekuBabaDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Giant spin    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Master spin   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Kokiri jump   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant jump    */ DMG_ENTRY(8, DEKUBABA_DMGEFF_SWORD),
    /* Master jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Unknown 1     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
};

// The only difference is that for Big Deku Babas, Hookshot will act the same as Deku Nuts: i.e. it will stun, but
// cannot kill.
static DamageTable sBigDekuBabaDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Kokiri sword  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Giant spin    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Master spin   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Kokiri jump   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant jump    */ DMG_ENTRY(8, DEKUBABA_DMGEFF_SWORD),
    /* Master jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Unknown 1     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 1500, ICHAIN_STOP),
};

void EnDekubaba_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDekubaba* thisv = (EnDekubaba*)thisx;
    s32 i;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 22.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gDekuBabaSkel, &gDekuBabaFastChompAnim, thisv->jointTable,
                   thisv->morphTable, 8);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderElements);

    if (thisv->actor.params == DEKUBABA_BIG) {
        thisv->size = 2.5f;

        for (i = 0; i < sJntSphInit.count; i++) {
            thisv->collider.elements[i].dim.worldSphere.radius = thisv->collider.elements[i].dim.modelSphere.radius =
                (sJntSphElementsInit[i].dim.modelSphere.radius * 2.50f);
        }

        // This and its counterpart below mean that a Deku Stick jumpslash will not trigger the Deku Stick drop route.
        // (Of course they reckoned without each age being able to use the other's items, so Stick and Master Sword
        // jumpslash can give the Stick drop as adult, and neither will as child.)
        if (!LINK_IS_ADULT) {
            sBigDekuBabaDamageTable.table[0x1B] = DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE); // DMG_JUMP_MASTER
        }

        CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sBigDekuBabaDamageTable, &sColChkInfoInit);
        thisv->actor.colChkInfo.health = 4;
        thisv->actor.naviEnemyId = 0x08; // Big Deku Baba
        thisv->actor.targetMode = 2;
    } else {
        thisv->size = 1.0f;

        for (i = 0; i < sJntSphInit.count; i++) {
            thisv->collider.elements[i].dim.worldSphere.radius = thisv->collider.elements[i].dim.modelSphere.radius;
        }

        if (!LINK_IS_ADULT) {
            sDekuBabaDamageTable.table[0x1B] = DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE); // DMG_JUMP_MASTER
        }

        CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDekuBabaDamageTable, &sColChkInfoInit);
        thisv->actor.naviEnemyId = 0x07; // Deku Baba
        thisv->actor.targetMode = 1;
    }

    EnDekubaba_SetupWait(thisv);
    thisv->timer = 0;
    thisv->boundFloor = NULL;
    thisv->bodyPartsPos[3] = thisv->actor.home.pos;
}

void EnDekubaba_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDekubaba* thisv = (EnDekubaba*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnDekubaba_DisableHitboxes(EnDekubaba* thisv) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(thisv->colliderElements); i++) {
        thisv->collider.elements[i].info.bumperFlags &= ~BUMP_ON;
    }
}

void EnDekubaba_SetupWait(EnDekubaba* thisv) {
    s32 i;
    ColliderJntSphElement* element;

    thisv->actor.shape.rot.x = -0x4000;
    thisv->stemSectionAngle[0] = thisv->stemSectionAngle[1] = thisv->stemSectionAngle[2] = thisv->actor.shape.rot.x;

    thisv->actor.world.pos.x = thisv->actor.home.pos.x;
    thisv->actor.world.pos.z = thisv->actor.home.pos.z;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 14.0f * thisv->size;

    Actor_SetScale(&thisv->actor, thisv->size * 0.01f * 0.5f);

    thisv->collider.base.colType = COLTYPE_HARD;
    thisv->collider.base.acFlags |= AC_HARD;
    thisv->timer = 45;

    for (i = 1; i < ARRAY_COUNT(thisv->colliderElements); i++) {
        element = &thisv->collider.elements[i];
        element->dim.worldSphere.center.x = thisv->actor.world.pos.x;
        element->dim.worldSphere.center.y = (s16)thisv->actor.world.pos.y - 7;
        element->dim.worldSphere.center.z = thisv->actor.world.pos.z;
    }

    thisv->actionFunc = EnDekubaba_Wait;
}

void EnDekubaba_SetupGrow(EnDekubaba* thisv) {
    s32 i;

    Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim) * (1.0f / 15), 0.0f,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_ONCE, 0.0f);

    thisv->timer = 15;

    for (i = 2; i < ARRAY_COUNT(thisv->colliderElements); i++) {
        thisv->collider.elements[i].info.ocElemFlags |= OCELEM_ON;
    }

    thisv->collider.base.colType = COLTYPE_HIT6;
    thisv->collider.base.acFlags &= ~AC_HARD;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DUMMY482);
    thisv->actionFunc = EnDekubaba_Grow;
}

void EnDekubaba_SetupRetract(EnDekubaba* thisv) {
    s32 i;

    Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);

    thisv->timer = 15;

    for (i = 2; i < ARRAY_COUNT(thisv->colliderElements); i++) {
        thisv->collider.elements[i].info.ocElemFlags &= ~OCELEM_ON;
    }

    thisv->actionFunc = EnDekubaba_Retract;
}

void EnDekubaba_SetupDecideLunge(EnDekubaba* thisv) {
    thisv->timer = Animation_GetLastFrame(&gDekuBabaFastChompAnim) * 2;
    Animation_MorphToLoop(&thisv->skelAnime, &gDekuBabaFastChompAnim, -3.0f);
    thisv->actionFunc = EnDekubaba_DecideLunge;
}

void EnDekubaba_SetupPrepareLunge(EnDekubaba* thisv) {
    thisv->timer = 8;
    thisv->skelAnime.playSpeed = 0.0f;
    thisv->actionFunc = EnDekubaba_PrepareLunge;
}

void EnDekubaba_SetupLunge(EnDekubaba* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gDekuBabaPauseChompAnim);
    thisv->timer = 0;
    thisv->actionFunc = EnDekubaba_Lunge;
}

void EnDekubaba_SetupPullBack(EnDekubaba* thisv) {
    Animation_Change(&thisv->skelAnime, &gDekuBabaPauseChompAnim, 1.0f, 15.0f,
                     Animation_GetLastFrame(&gDekuBabaPauseChompAnim), ANIMMODE_ONCE, -3.0f);
    thisv->timer = 0;
    thisv->actionFunc = EnDekubaba_PullBack;
}

void EnDekubaba_SetupRecover(EnDekubaba* thisv) {
    thisv->timer = 9;
    thisv->collider.base.acFlags |= AC_ON;
    thisv->skelAnime.playSpeed = -1.0f;
    thisv->actionFunc = EnDekubaba_Recover;
}

void EnDekubaba_SetupHit(EnDekubaba* thisv, s32 arg1) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gDekuBabaPauseChompAnim, -5.0f);
    thisv->timer = arg1;
    thisv->collider.base.acFlags &= ~AC_ON;
    Actor_SetScale(&thisv->actor, thisv->size * 0.01f);

    if (arg1 == 2) {
        Actor_SetColorFilter(&thisv->actor, 0, 155, 0, 62);
    } else {
        Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 42);
    }

    thisv->actionFunc = EnDekubaba_Hit;
}

void EnDekubaba_SetupPrunedSomersault(EnDekubaba* thisv) {
    thisv->timer = 0;
    thisv->skelAnime.playSpeed = 0.0f;
    thisv->actor.gravity = -0.8f;
    thisv->actor.velocity.y = 4.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x8000;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.speedXZ = thisv->size * 3.0f;
    thisv->actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    thisv->actionFunc = EnDekubaba_PrunedSomersault;
}

void EnDekubaba_SetupShrinkDie(EnDekubaba* thisv) {
    Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnDekubaba_ShrinkDie;
}

void EnDekubaba_SetupStunnedVertical(EnDekubaba* thisv) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(thisv->colliderElements); i++) {
        thisv->collider.elements[i].info.bumperFlags |= BUMP_ON;
    }

    if (thisv->timer == 1) {
        Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim, 4.0f, 0.0f,
                         Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        thisv->timer = 40;
    } else {
        Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim, 0.0f, 0.0f,
                         Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        thisv->timer = 60;
    }

    thisv->actor.world.pos.x = thisv->actor.home.pos.x;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + (60.0f * thisv->size);
    thisv->actor.world.pos.z = thisv->actor.home.pos.z;
    thisv->actionFunc = EnDekubaba_StunnedVertical;
}

void EnDekubaba_SetupSway(EnDekubaba* thisv) {
    thisv->targetSwayAngle = -0x6000;
    thisv->stemSectionAngle[2] = -0x5000;
    thisv->stemSectionAngle[1] = -0x4800;

    EnDekubaba_DisableHitboxes(thisv);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 35);
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnDekubaba_Sway;
}

void EnDekubaba_SetupDeadStickDrop(EnDekubaba* thisv, GlobalContext* globalCtx) {
    Actor_SetScale(&thisv->actor, 0.03f);
    thisv->actor.shape.rot.x -= 0x4000;
    thisv->actor.shape.yOffset = 1000.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.shape.shadowScale = 3.0f;
    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_MISC);
    thisv->actor.flags &= ~ACTOR_FLAG_5;
    thisv->timer = 200;
    thisv->actionFunc = EnDekubaba_DeadStickDrop;
}

// Action functions

void EnDekubaba_Wait(EnDekubaba* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.world.pos.x = thisv->actor.home.pos.x;
    thisv->actor.world.pos.z = thisv->actor.home.pos.z;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 14.0f * thisv->size;

    if ((thisv->timer == 0) && (thisv->actor.xzDistToPlayer < 200.0f * thisv->size) &&
        (fabsf(thisv->actor.yDistToPlayer) < 30.0f * thisv->size)) {
        EnDekubaba_SetupGrow(thisv);
    }
}

void EnDekubaba_Grow(EnDekubaba* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 headShiftX;
    f32 headShiftZ;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    SkelAnime_Update(&thisv->skelAnime);

    thisv->actor.scale.x = thisv->actor.scale.y = thisv->actor.scale.z =
        thisv->size * 0.01f * (0.5f + (15 - thisv->timer) * 0.5f / 15.0f);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x1800, 0x800);

    headDistVertical = sinf(CLAMP_MAX((15 - thisv->timer) * (1.0f / 15), 0.7f) * std::numbers::pi_v<float>) * 32.0f + 14.0f;

    if (thisv->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (thisv->actor.shape.rot.x < -0x238E) {
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x5555, 0x38E);
        headDistHorizontal = Math_CosS(thisv->stemSectionAngle[0]) * 20.0f;
    } else if (thisv->actor.shape.rot.x < -0xE38) {
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0xAAA, 0x38E);
        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x5555, 0x38E);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (Math_CosS(thisv->stemSectionAngle[0]) + Math_CosS(thisv->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(thisv->stemSectionAngle[0]) - Math_SinS(thisv->stemSectionAngle[1]))) *
                                 Math_CosS(thisv->stemSectionAngle[2]) / -Math_SinS(thisv->stemSectionAngle[2]);
    } else {
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0xAAA, 0x38E);
        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x31C7, 0x222);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (Math_CosS(thisv->stemSectionAngle[0]) + Math_CosS(thisv->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(thisv->stemSectionAngle[0]) - Math_SinS(thisv->stemSectionAngle[1]))) *
                                 Math_CosS(thisv->stemSectionAngle[2]) / -Math_SinS(thisv->stemSectionAngle[2]);
    }

    if (thisv->timer < 10) {
        Math_ApproachS(&thisv->actor.shape.rot.y, Math_Vec3f_Yaw(&thisv->actor.home.pos, &player->actor.world.pos), 2,
                       0xE38);
        if (headShiftZ) {} // One way of fake-matching
    }

    thisv->actor.world.pos.y = thisv->actor.home.pos.y + (headDistVertical * thisv->size);
    headShiftX = headDistHorizontal * thisv->size * Math_SinS(thisv->actor.shape.rot.y);
    headShiftZ = headDistHorizontal * thisv->size * Math_CosS(thisv->actor.shape.rot.y);
    thisv->actor.world.pos.x = thisv->actor.home.pos.x + headShiftX;
    thisv->actor.world.pos.z = thisv->actor.home.pos.z + headShiftZ;

    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.home.pos, thisv->size * 3.0f, 0, thisv->size * 12.0f,
                             thisv->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

    if (thisv->timer == 0) {
        if (Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < 240.0f * thisv->size) {
            EnDekubaba_SetupPrepareLunge(thisv);
        } else {
            EnDekubaba_SetupRetract(thisv);
        }
    }
}

void EnDekubaba_Retract(EnDekubaba* thisv, GlobalContext* globalCtx) {
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 xShift;
    f32 zShift;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    SkelAnime_Update(&thisv->skelAnime);

    thisv->actor.scale.x = thisv->actor.scale.y = thisv->actor.scale.z =
        thisv->size * 0.01f * (0.5f + thisv->timer * (1.0f / 30));
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x4000, 0x300);

    headDistVertical = (sinf(CLAMP_MAX(thisv->timer * 0.033f, 0.7f) * std::numbers::pi_v<float>) * 32.0f) + 14.0f;

    if (thisv->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (thisv->actor.shape.rot.x < -0x238E) {
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x4000, 0x555);
        headDistHorizontal = Math_CosS(thisv->stemSectionAngle[0]) * 20.0f;
    } else if (thisv->actor.shape.rot.x < -0xE38) {
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x5555, 0x555);
        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x4000, 0x555);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (Math_CosS(thisv->stemSectionAngle[0]) + Math_CosS(thisv->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(thisv->stemSectionAngle[0]) - Math_SinS(thisv->stemSectionAngle[1]))) *
                                 Math_CosS(thisv->stemSectionAngle[2]) / -Math_SinS(thisv->stemSectionAngle[2]);
    } else {
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x5555, 0x555);
        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x5555, 0x333);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (Math_CosS(thisv->stemSectionAngle[0]) + Math_CosS(thisv->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(thisv->stemSectionAngle[0]) - Math_SinS(thisv->stemSectionAngle[1]))) *
                                 Math_CosS(thisv->stemSectionAngle[2]) / -Math_SinS(thisv->stemSectionAngle[2]);
    }

    thisv->actor.world.pos.y = thisv->actor.home.pos.y + (headDistVertical * thisv->size);
    xShift = headDistHorizontal * thisv->size * Math_SinS(thisv->actor.shape.rot.y);
    zShift = headDistHorizontal * thisv->size * Math_CosS(thisv->actor.shape.rot.y);
    thisv->actor.world.pos.x = thisv->actor.home.pos.x + xShift;
    thisv->actor.world.pos.z = thisv->actor.home.pos.z + zShift;

    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.home.pos, thisv->size * 3.0f, 0, thisv->size * 12.0f,
                             thisv->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 0xA, NULL);

    if (thisv->timer == 0) {
        EnDekubaba_SetupWait(thisv);
    }
}

void EnDekubaba_UpdateHeadPosition(EnDekubaba* thisv) {
    f32 horizontalHeadShift = (Math_CosS(thisv->stemSectionAngle[0]) + Math_CosS(thisv->stemSectionAngle[1]) +
                               Math_CosS(thisv->stemSectionAngle[2])) *
                              20.0f;

    thisv->actor.world.pos.x =
        thisv->actor.home.pos.x + Math_SinS(thisv->actor.shape.rot.y) * (horizontalHeadShift * thisv->size);
    thisv->actor.world.pos.y =
        thisv->actor.home.pos.y - (Math_SinS(thisv->stemSectionAngle[0]) + Math_SinS(thisv->stemSectionAngle[1]) +
                                  Math_SinS(thisv->stemSectionAngle[2])) *
                                     20.0f * thisv->size;
    thisv->actor.world.pos.z =
        thisv->actor.home.pos.z + Math_CosS(thisv->actor.shape.rot.y) * (horizontalHeadShift * thisv->size);
}

void EnDekubaba_DecideLunge(EnDekubaba* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
        if (thisv->actor.params == DEKUBABA_BIG) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_MOUTH);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_JR_MOUTH);
        }
    }

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    Math_ApproachS(&thisv->actor.shape.rot.y, Math_Vec3f_Yaw(&thisv->actor.home.pos, &player->actor.world.pos), 2,
                   (thisv->timer % 5) * 0x222);

    if (thisv->timer < 10) {
        thisv->stemSectionAngle[0] += 0x16C;
        thisv->stemSectionAngle[1] += 0x16C;
        thisv->stemSectionAngle[2] += 0xB6;
        thisv->actor.shape.rot.x += 0x222;
    } else if (thisv->timer < 20) {
        thisv->stemSectionAngle[0] -= 0x16C;
        thisv->stemSectionAngle[1] += 0x111;
        thisv->actor.shape.rot.x += 0x16C;
    } else if (thisv->timer < 30) {
        thisv->stemSectionAngle[1] -= 0x111;
        thisv->actor.shape.rot.x -= 0xB6;
    } else {
        thisv->stemSectionAngle[1] -= 0xB6;
        thisv->stemSectionAngle[2] += 0xB6;
        thisv->actor.shape.rot.x -= 0x16C;
    }

    EnDekubaba_UpdateHeadPosition(thisv);

    if (240.0f * thisv->size < Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos)) {
        EnDekubaba_SetupRetract(thisv);
    } else if ((thisv->timer == 0) || (thisv->actor.xzDistToPlayer < 80.0f * thisv->size)) {
        EnDekubaba_SetupPrepareLunge(thisv);
    }
}

void EnDekubaba_Lunge(EnDekubaba* thisv, GlobalContext* globalCtx) {
    static Color_RGBA8 primColor = { 105, 255, 105, 255 };
    static Color_RGBA8 envColor = { 150, 250, 150, 0 };
    s32 allStepsDone;
    s16 curFrame10;
    Vec3f velocity;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer == 0) {
        if (Animation_OnFrame(&thisv->skelAnime, 1.0f)) {
            if (thisv->actor.params == DEKUBABA_BIG) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_ATTACK);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_JR_ATTACK);
            }
        }

        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x222);

        curFrame10 = thisv->skelAnime.curFrame * 10.0f;

        allStepsDone = true;
        allStepsDone &= Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0xE38, curFrame10 + 0x38E);
        allStepsDone &= Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0xE38, curFrame10 + 0x71C);
        allStepsDone &= Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0xE38, curFrame10 + 0xE38);

        if (allStepsDone) {
            Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gDekuBabaFastChompAnim, 4.0f);
            velocity.x = Math_SinS(thisv->actor.shape.rot.y) * 5.0f;
            velocity.y = 0.0f;
            velocity.z = Math_CosS(thisv->actor.shape.rot.y) * 5.0f;

            func_8002829C(globalCtx, &thisv->actor.world.pos, &velocity, &sZeroVec, &primColor, &envColor, 1,
                          thisv->size * 100.0f);
            thisv->timer = 1;
            thisv->collider.base.acFlags |= AC_ON;
        }
    } else if (thisv->timer > 10) {
        EnDekubaba_SetupPullBack(thisv);
    } else {
        thisv->timer++;

        if ((thisv->timer >= 4) && !Actor_IsFacingPlayer(&thisv->actor, 0x16C)) {
            Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xF, 0x71C);
        }

        if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
            if (thisv->actor.params == DEKUBABA_BIG) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_MOUTH);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_JR_MOUTH);
            }
        }
    }

    EnDekubaba_UpdateHeadPosition(thisv);
}

void EnDekubaba_PrepareLunge(EnDekubaba* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0x1800, 2, 0xE38, 0x71C);
    Math_ApproachS(&thisv->actor.shape.rot.y, Math_Vec3f_Yaw(&thisv->actor.home.pos, &player->actor.world.pos), 2, 0xE38);
    Math_ScaledStepToS(&thisv->stemSectionAngle[0], 0xAAA, 0x444);
    Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x4718, 0x888);
    Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x6AA4, 0x888);

    if (thisv->timer == 0) {
        EnDekubaba_SetupLunge(thisv);
    }

    EnDekubaba_UpdateHeadPosition(thisv);
}

void EnDekubaba_PullBack(EnDekubaba* thisv, GlobalContext* globalCtx) {
    Vec3f dustPos;
    f32 xIncr;
    f32 zIncr;
    s32 i;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer == 0) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x93E, 0x38E);
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x888, 0x16C);
        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x888, 0x16C);
        if (Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x888, 0x16C)) {
            xIncr = Math_SinS(thisv->actor.shape.rot.y) * 30.0f * thisv->size;
            zIncr = Math_CosS(thisv->actor.shape.rot.y) * 30.0f * thisv->size;
            dustPos = thisv->actor.home.pos;

            for (i = 0; i < 3; i++) {
                func_800286CC(globalCtx, &dustPos, &sZeroVec, &sZeroVec, thisv->size * 500.0f, thisv->size * 50.0f);
                dustPos.x += xIncr;
                dustPos.z += zIncr;
            }

            thisv->timer = 1;
        }
    } else if (thisv->timer == 11) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x93E, 0x200);
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0xAAA, 0x200);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x5C71, 0x200);

        if (Math_ScaledStepToS(&thisv->stemSectionAngle[1], 0x238C, 0x200)) {
            thisv->timer = 12;
        }
    } else if (thisv->timer == 18) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x2AA8, 0xAAA);

        if (Math_ScaledStepToS(&thisv->stemSectionAngle[0], 0x1554, 0x5B0)) {
            thisv->timer = 25;
        }

        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x38E3, 0xAAA);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x5C71, 0x2D8);
    } else if (thisv->timer == 25) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x5550, 0xAAA);

        if (Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x6388, 0x93E)) {
            thisv->timer = 26;
        }

        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x3FFC, 0x4FA);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x238C, 0x444);
    } else if (thisv->timer == 26) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x1800, 0x93E);

        if (Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x1555, 0x71C)) {
            thisv->timer = 27;
        }

        Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x38E3, 0x2D8);
        Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x5C71, 0x5B0);
    } else if (thisv->timer >= 27) {
        thisv->timer++;

        if (thisv->timer > 30) {
            if (thisv->actor.xzDistToPlayer < 80.0f * thisv->size) {
                EnDekubaba_SetupPrepareLunge(thisv);
            } else {
                EnDekubaba_SetupDecideLunge(thisv);
            }
        }
    } else {
        thisv->timer++;

        if (thisv->timer == 10) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_SCRAPE);
        }

        if (thisv->timer >= 12) {
            Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x5C71, 0x88);
        }
    }

    EnDekubaba_UpdateHeadPosition(thisv);
}

void EnDekubaba_Recover(EnDekubaba* thisv, GlobalContext* globalCtx) {
    s32 anyStepsDone;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer > 8) {
        anyStepsDone = Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0x1800, 1, 0x11C6, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&thisv->stemSectionAngle[0], -0x1555, 1, 0xAAA, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&thisv->stemSectionAngle[1], -0x38E3, 1, 0xE38, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&thisv->stemSectionAngle[2], -0x5C71, 1, 0x11C6, 0x71C);

        if (!anyStepsDone) {
            thisv->timer = 8;
        }
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            EnDekubaba_SetupDecideLunge(thisv);
        }
    }

    EnDekubaba_UpdateHeadPosition(thisv);
}

/**
 * Hit by a weapon or hit something when lunging.
 */
void EnDekubaba_Hit(EnDekubaba* thisv, GlobalContext* globalCtx) {
    s32 allStepsDone;

    SkelAnime_Update(&thisv->skelAnime);

    allStepsDone = true;
    allStepsDone &= Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&thisv->stemSectionAngle[0], -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&thisv->stemSectionAngle[1], -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&thisv->stemSectionAngle[2], -0x4000, 0xE38);

    if (allStepsDone) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnDekubaba_SetupShrinkDie(thisv);
        } else {
            thisv->collider.base.acFlags |= AC_ON;
            if (thisv->timer == 0) {
                if (thisv->actor.xzDistToPlayer < 80.0f * thisv->size) {
                    EnDekubaba_SetupPrepareLunge(thisv);
                } else {
                    EnDekubaba_SetupRecover(thisv);
                }
            } else {
                EnDekubaba_SetupStunnedVertical(thisv);
            }
        }
    }

    EnDekubaba_UpdateHeadPosition(thisv);
}

void EnDekubaba_StunnedVertical(EnDekubaba* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        EnDekubaba_DisableHitboxes(thisv);

        if (thisv->actor.xzDistToPlayer < 80.0f * thisv->size) {
            EnDekubaba_SetupPrepareLunge(thisv);
        } else {
            EnDekubaba_SetupRecover(thisv);
        }
    }
}

/**
 * Sway back and forth with decaying amplitude until close enough to vertical.
 */
void EnDekubaba_Sway(EnDekubaba* thisv, GlobalContext* globalCtx) {
    s16 angleToVertical;

    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, thisv->stemSectionAngle[0], 0x71C);
    Math_ScaledStepToS(&thisv->stemSectionAngle[0], thisv->stemSectionAngle[1], 0x71C);
    Math_ScaledStepToS(&thisv->stemSectionAngle[1], thisv->stemSectionAngle[2], 0x71C);

    if (Math_ScaledStepToS(&thisv->stemSectionAngle[2], thisv->targetSwayAngle, 0x71C)) {
        thisv->targetSwayAngle = -0x4000 - (thisv->targetSwayAngle + 0x4000) * 0.8f;
    }
    angleToVertical = thisv->targetSwayAngle + 0x4000;

    if (ABS(angleToVertical) < 0x100) {
        thisv->collider.base.acFlags |= AC_ON;
        if (thisv->actor.xzDistToPlayer < 80.0f * thisv->size) {
            EnDekubaba_SetupPrepareLunge(thisv);
        } else {
            EnDekubaba_SetupRecover(thisv);
        }
    }

    EnDekubaba_UpdateHeadPosition(thisv);
}

void EnDekubaba_PrunedSomersault(EnDekubaba* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f dustPos;
    f32 deltaX;
    f32 deltaZ;
    f32 deltaY;

    Math_StepToF(&thisv->actor.speedXZ, 0.0f, thisv->size * 0.1f);

    if (thisv->timer == 0) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x4800, 0x71C);
        Math_ScaledStepToS(&thisv->stemSectionAngle[0], 0x4800, 0x71C);
        Math_ScaledStepToS(&thisv->stemSectionAngle[1], 0x4800, 0x71C);

        EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, thisv->size * 3.0f, 0, thisv->size * 12.0f,
                                 thisv->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

        if ((thisv->actor.scale.x > 0.005f) && ((thisv->actor.bgCheckFlags & 2) || (thisv->actor.bgCheckFlags & 8))) {
            thisv->actor.scale.x = thisv->actor.scale.y = thisv->actor.scale.z = 0.0f;
            thisv->actor.speedXZ = 0.0f;
            thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
            EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, thisv->size * 3.0f, 0, thisv->size * 12.0f,
                                     thisv->size * 5.0f, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        }

        if (thisv->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
            thisv->timer = 1;
        }
    } else if (thisv->timer == 1) {
        dustPos = thisv->actor.world.pos;

        deltaY = 20.0f * Math_SinS(thisv->actor.shape.rot.x);
        deltaX = -20.0f * Math_CosS(thisv->actor.shape.rot.x) * Math_SinS(thisv->actor.shape.rot.y);
        deltaZ = -20.0f * Math_CosS(thisv->actor.shape.rot.x) * Math_CosS(thisv->actor.shape.rot.y);

        for (i = 0; i < 4; i++) {
            func_800286CC(globalCtx, &dustPos, &sZeroVec, &sZeroVec, 500, 50);
            dustPos.x += deltaX;
            dustPos.y += deltaY;
            dustPos.z += deltaZ;
        }

        func_800286CC(globalCtx, &thisv->actor.home.pos, &sZeroVec, &sZeroVec, thisv->size * 500.0f, thisv->size * 100.0f);
        EnDekubaba_SetupDeadStickDrop(thisv, globalCtx);
    }
}

/**
 * Die and drop Deku Nuts (Stick drop is handled elsewhere)
 */
void EnDekubaba_ShrinkDie(EnDekubaba* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, thisv->size * 5.0f);

    if (Math_StepToF(&thisv->actor.scale.x, thisv->size * 0.1f * 0.01f, thisv->size * 0.1f * 0.01f)) {
        func_800286CC(globalCtx, &thisv->actor.home.pos, &sZeroVec, &sZeroVec, thisv->size * 500.0f, thisv->size * 100.0f);
        if (thisv->actor.dropFlag == 0) {
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_NUTS);

            if (thisv->actor.params == DEKUBABA_BIG) {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_NUTS);
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_NUTS);
            }
        } else {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x30);
        }
        Actor_Kill(&thisv->actor);
    }

    thisv->actor.scale.y = thisv->actor.scale.z = thisv->actor.scale.x;
    thisv->actor.shape.rot.z += 0x1C70;
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.home.pos, thisv->size * 3.0f, 0, thisv->size * 12.0f,
                             thisv->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void EnDekubaba_DeadStickDrop(EnDekubaba* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (Actor_HasParent(&thisv->actor, globalCtx) || (thisv->timer == 0)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    func_8002F554(&thisv->actor, globalCtx, GI_STICKS_1);
}

// Update and associated functions

void EnDekubaba_UpdateDamage(EnDekubaba* thisv, GlobalContext* globalCtx) {
    Vec3f* firePos;
    f32 fireScale;
    s32 phi_s0; // Used for both health and iterator

    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&thisv->actor, &thisv->collider, 1);

        if ((thisv->collider.base.colType != COLTYPE_HARD) &&
            ((thisv->actor.colChkInfo.damageEffect != DEKUBABA_DMGEFF_NONE) || (thisv->actor.colChkInfo.damage != 0))) {

            phi_s0 = thisv->actor.colChkInfo.health - thisv->actor.colChkInfo.damage;

            if (thisv->actionFunc != EnDekubaba_StunnedVertical) {
                if ((thisv->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) ||
                    (thisv->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_DEKUNUT)) {
                    if (thisv->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) {
                        phi_s0 = thisv->actor.colChkInfo.health;
                    }

                    EnDekubaba_SetupHit(thisv, 2);
                } else if (thisv->actionFunc == EnDekubaba_PullBack) {
                    if (phi_s0 <= 0) {
                        phi_s0 = 1;
                    }

                    EnDekubaba_SetupHit(thisv, 1);
                } else {
                    EnDekubaba_SetupHit(thisv, 0);
                }
            } else if ((thisv->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) ||
                       (thisv->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_SWORD)) {
                if (phi_s0 > 0) {
                    EnDekubaba_SetupSway(thisv);
                } else {
                    EnDekubaba_SetupPrunedSomersault(thisv);
                }
            } else if (thisv->actor.colChkInfo.damageEffect != DEKUBABA_DMGEFF_DEKUNUT) {
                EnDekubaba_SetupHit(thisv, 0);
            } else {
                return;
            }

            thisv->actor.colChkInfo.health = CLAMP_MIN(phi_s0, 0);

            if (thisv->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_FIRE) {
                firePos = &thisv->actor.world.pos;
                fireScale = (thisv->size * 70.0f);

                for (phi_s0 = 0; phi_s0 < 4; phi_s0++) {
                    EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, firePos, fireScale, 0, 0, phi_s0);
                }
            }
        } else {
            return;
        }
    } else if ((globalCtx->actorCtx.unk_02 != 0) && (thisv->collider.base.colType != COLTYPE_HARD) &&
               (thisv->actionFunc != EnDekubaba_StunnedVertical) && (thisv->actionFunc != EnDekubaba_Hit) &&
               (thisv->actor.colChkInfo.health != 0)) {
        thisv->actor.colChkInfo.health--;
        thisv->actor.dropFlag = 0x00;
        EnDekubaba_SetupHit(thisv, 1);
    } else {
        return;
    }

    if (thisv->actor.colChkInfo.health != 0) {
        if (thisv->timer == 2) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_DAMAGE);
        }
    } else {
        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
        if (thisv->actor.params == DEKUBABA_BIG) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_DEAD);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_JR_DEAD);
        }
    }
}

void EnDekubaba_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDekubaba* thisv = (EnDekubaba*)thisx;

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        EnDekubaba_SetupRecover(thisv);
    }

    EnDekubaba_UpdateDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actionFunc == EnDekubaba_PrunedSomersault) {
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, thisv->size * 15.0f, 10.0f, 5);
    } else if (thisv->actionFunc != EnDekubaba_DeadStickDrop) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
        if (thisv->boundFloor == NULL) {
            thisv->boundFloor = thisv->actor.floorPoly;
        }
    }
    if (thisv->actionFunc == EnDekubaba_Lunge) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        thisv->actor.flags |= ACTOR_FLAG_24;
    }

    if (thisv->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if (thisv->actionFunc != EnDekubaba_DeadStickDrop) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

// Draw functions

void EnDekubaba_DrawStemRetracted(EnDekubaba* thisv, GlobalContext* globalCtx) {
    f32 horizontalScale;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2445);

    horizontalScale = thisv->size * 0.01f;

    Matrix_Translate(thisv->actor.home.pos.x, thisv->actor.home.pos.y + (-6.0f * thisv->size), thisv->actor.home.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateZYX(thisv->stemSectionAngle[0], thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_Scale(horizontalScale, horizontalScale, horizontalScale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2461),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemTopDL);

    Actor_SetFocus(&thisv->actor, 0.0f);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2468);
}

void EnDekubaba_DrawStemExtended(EnDekubaba* thisv, GlobalContext* globalCtx) {
    static const Gfx* stemDLists[] = { gDekuBabaStemTopDL, gDekuBabaStemMiddleDL, gDekuBabaStemBaseDL };
    MtxF mtx;
    s32 i;
    f32 horizontalStepSize;
    f32 spA4;
    f32 scale;
    s32 stemSections;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2487);

    if (thisv->actionFunc == EnDekubaba_PrunedSomersault) {
        stemSections = 2;
    } else {
        stemSections = 3;
    }

    scale = thisv->size * 0.01f;
    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_Get(&mtx);
    if (thisv->actor.colorFilterTimer != 0) {
        spA4 = thisv->size * 20.0f;
        thisv->bodyPartsPos[2].x = thisv->actor.world.pos.x;
        thisv->bodyPartsPos[2].y = thisv->actor.world.pos.y - spA4;
        thisv->bodyPartsPos[2].z = thisv->actor.world.pos.z;
    }

    for (i = 0; i < stemSections; i++) {
        mtx.mf_raw.yw += 20.0f * Math_SinS(thisv->stemSectionAngle[i]) * thisv->size;
        horizontalStepSize = 20.0f * Math_CosS(thisv->stemSectionAngle[i]) * thisv->size;
        mtx.mf_raw.xw -= horizontalStepSize * Math_SinS(thisv->actor.shape.rot.y);
        mtx.mf_raw.zw -= horizontalStepSize * Math_CosS(thisv->actor.shape.rot.y);

        Matrix_Put(&mtx);
        Matrix_RotateZYX(thisv->stemSectionAngle[i], thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2533),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPDisplayList(POLY_OPA_DISP++, stemDLists[i]);

        Collider_UpdateSpheres(51 + 2 * i, &thisv->collider);
        Collider_UpdateSpheres(52 + 2 * i, &thisv->collider);

        if (i == 0) {
            if (thisv->actionFunc != EnDekubaba_Sway) {
                thisv->actor.focus.pos.x = mtx.mf_raw.xw;
                thisv->actor.focus.pos.y = mtx.mf_raw.yw;
                thisv->actor.focus.pos.z = mtx.mf_raw.zw;
            } else {
                thisv->actor.focus.pos.x = thisv->actor.home.pos.x;
                thisv->actor.focus.pos.y = thisv->actor.home.pos.y + (40.0f * thisv->size);
                thisv->actor.focus.pos.z = thisv->actor.home.pos.z;
            }
        }

        if ((i < 2) && (thisv->actor.colorFilterTimer != 0)) {
            // checking colorFilterTimer ensures that spA4 has been initialized earlier, so not a bug
            thisv->bodyPartsPos[i].x = mtx.mf_raw.xw;
            thisv->bodyPartsPos[i].y = mtx.mf_raw.yw - spA4;
            thisv->bodyPartsPos[i].z = mtx.mf_raw.zw;
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2569);
}

void EnDekubaba_DrawStemBasePruned(EnDekubaba* thisv, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2579);

    Matrix_RotateZYX(thisv->stemSectionAngle[2], thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2586),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemBaseDL);

    Collider_UpdateSpheres(55, &thisv->collider);
    Collider_UpdateSpheres(56, &thisv->collider);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2596);
}

void EnDekubaba_DrawBaseShadow(EnDekubaba* thisv, GlobalContext* globalCtx) {
    MtxF mtx;
    f32 horizontalScale;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2692);
    func_80094044(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    func_80038A28(thisv->boundFloor, thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, &mtx);
    Matrix_Mult(&mtx, MTXMODE_NEW);

    horizontalScale = thisv->size * 0.15f;
    Matrix_Scale(horizontalScale, 1.0f, horizontalScale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2710),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2715);
}

void EnDekubaba_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnDekubaba* thisv = (EnDekubaba*)thisx;

    if (limbIndex == 1) {
        Collider_UpdateSpheres(limbIndex, &thisv->collider);
    }
}

void EnDekubaba_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDekubaba* thisv = (EnDekubaba*)thisx;
    f32 scale;

    if (1) {}
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2752);
    func_80093D18(globalCtx->state.gfxCtx);

    if (thisv->actionFunc != EnDekubaba_DeadStickDrop) {
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL,
                          EnDekubaba_PostLimbDraw, thisv);

        if (thisv->actionFunc == EnDekubaba_Wait) {
            EnDekubaba_DrawStemRetracted(thisv, globalCtx);
        } else {
            EnDekubaba_DrawStemExtended(thisv, globalCtx);
        }

        scale = thisv->size * 0.01f;
        Matrix_Translate(thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, MTXMODE_NEW);
        Matrix_RotateY(thisv->actor.home.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2780),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaBaseLeavesDL);

        if (thisv->actionFunc == EnDekubaba_PrunedSomersault) {
            EnDekubaba_DrawStemBasePruned(thisv, globalCtx);
        }

        if (thisv->boundFloor != NULL) {
            EnDekubaba_DrawBaseShadow(thisv, globalCtx);
        }

        // Display solid until 40 frames left, then blink until killed.
    } else if ((thisv->timer > 40) || ((thisv->timer % 2) != 0)) {
        Matrix_Translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2797),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStickDropDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dekubaba.c", 2804);
}

//OTRTODO fix thisv one
void EnDekuBaba_Reset(void) {
    // DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD)
    
}