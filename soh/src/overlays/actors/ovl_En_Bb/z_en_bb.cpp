/*
 * File: z_en_bb.c
 * Overlay: ovl_En_Bb
 * Description: Bubble (Flying Skull Enemy)
 */

#include "z_en_bb.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_Bb/object_Bb.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_24)

#define vBombHopPhase actionVar1
#define vTrailIdx actionVar1
#define vTrailMaxAlpha actionVar2
#define vMoveAngleY actionVar2
#define vFlameTimer actionVar2

typedef enum {
    /* 0 */ BB_DAMAGE,
    /* 1 */ BB_KILL,
    /* 2 */ BB_FLAME_TRAIL,
    /* 3 */ BB_DOWN,
    /* 4 */ BB_STUNNED,
    /* 5 */ BB_UNUSED,
    /* 6 */ BB_BLUE,
    /* 7 */ BB_RED,
    /* 8 */ BB_WHITE,
    /* 9 */ BB_GREEN
} EnBbAction;

typedef enum {
    /* 0 */ BBMOVE_NORMAL,
    /* 1 */ BBMOVE_NOCLIP,
    /* 2 */ BBMOVE_HIDDEN
} EnBbMoveMode;

typedef enum {
    /* 0 */ BBBLUE_NORMAL,
    /* 1 */ BBBLUE_AGGRO
} EnBbBlueActionState;

typedef enum {
    /* 0 */ BBRED_WAIT,
    /* 1 */ BBRED_ATTACK,
    /* 2 */ BBRED_HIDE
} EnBbRedActionState;

typedef enum {
    /* 0 */ BBGREEN_FLAME_ON,
    /* 1 */ BBGREEN_FLAME_OFF
} EnBbGreenActionState;

// Main functions

void EnBb_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBb_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBb_Update(Actor* thisv, GlobalContext* globalCtx);
void EnBb_Draw(Actor* thisv, GlobalContext* globalCtx);

// Helper functions

void EnBb_FaceWaypoint(EnBb* thisv);
void EnBb_SetWaypoint(EnBb* thisv, GlobalContext* globalCtx);

// Action functions

void EnBb_SetupFlameTrail(EnBb* thisv);
void EnBb_FlameTrail(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_SetupDeath(EnBb* thisv, GlobalContext* globalCtx);
void EnBb_Death(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_Damage(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_SetupBlue(EnBb* thisv);
void EnBb_Blue(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_SetupDown(EnBb* thisv);
void EnBb_Down(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_SetupRed(GlobalContext* globalCtx, EnBb* thisv);
void EnBb_Red(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_SetupWhite(GlobalContext* globalCtx, EnBb* thisv);
void EnBb_White(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_InitGreen(EnBb* thisv, GlobalContext* globalCtx);
void EnBb_Green(EnBb* thisv, GlobalContext* globalCtx);

void EnBb_Stunned(EnBb* thisv, GlobalContext* globalCtx);

static DamageTable sDamageTableBlueGreen = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(4, 0xC),
    /* Light arrow   */ DMG_ENTRY(4, 0xB),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0x9),
    /* Light magic   */ DMG_ENTRY(3, 0x8),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static DamageTable sDamageTableRed = {
    /* Deku nut      */ DMG_ENTRY(0, 0xD),
    /* Deku stick    */ DMG_ENTRY(0, 0xD),
    /* Slingshot     */ DMG_ENTRY(0, 0xD),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xD),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xD),
    /* Kokiri sword  */ DMG_ENTRY(0, 0xD),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(4, 0x9),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xE),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0x9),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static DamageTable sDamageTableWhite = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0xE),
    /* Slingshot     */ DMG_ENTRY(1, 0xE),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
    /* Fire arrow    */ DMG_ENTRY(4, 0x5),
    /* Ice arrow     */ DMG_ENTRY(2, 0xE),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xE),
    /* Fire magic    */ DMG_ENTRY(4, 0x7),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xE),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xE),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

const ActorInit En_Bb_InitVars = {
    ACTOR_EN_BB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BB,
    sizeof(EnBb),
    (ActorFunc)EnBb_Init,
    (ActorFunc)EnBb_Destroy,
    (ActorFunc)EnBb_Update,
    (ActorFunc)EnBb_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, -120, 0 }, 4 }, 300 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 10, ICHAIN_STOP),
};

void EnBb_SetupAction(EnBb* thisv, EnBbActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

Actor* EnBb_FindExplosive(GlobalContext* globalCtx, EnBb* thisv, f32 range) {
    Actor* explosive = globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    f32 dist;

    while (explosive != NULL) {
        if (explosive->params != 0) {
            explosive = explosive->next;
            continue;
        }
        dist = Actor_WorldDistXYZToActor(&thisv->actor, explosive);
        if ((explosive->params == 0) && (dist <= range)) {
            return explosive;
        }
        explosive = explosive->next;
    }
    return NULL;
}

void EnBb_SpawnFlameTrail(GlobalContext* globalCtx, EnBb* thisv, s16 startAtZero) {
    EnBb* now = thisv;
    EnBb* next;
    s32 i;

    for (i = 0; i < 5; i++) {
        next = (EnBb*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BB, thisv->actor.world.pos.x,
                                  thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
        if (next != NULL) {
            now->actor.child = &next->actor;
            next->actor.parent = &now->actor;
            next->targetActor = &thisv->actor;
            next->vTrailIdx = i + 1;
            next->actor.scale.x = 1.0f;
            next->vTrailMaxAlpha = next->flamePrimAlpha = 255 - (i * 40);
            next->flameScaleY = next->actor.scale.y = 0.8f - (i * 0.075f);
            next->flameScaleX = next->actor.scale.z = 1.0f - (i * 0.094f);
            if (startAtZero) {
                next->flamePrimAlpha = 0;
                next->flameScaleY = next->flameScaleX = 0.0f;
            }
            next->flameScrollMod = i + 1;
            next->timer = 2 * i + 2;
            next->flameEnvColor.r = 255;
            now = next;
        }
    }
}

void EnBb_KillFlameTrail(EnBb* thisv) {
    Actor* actor = &thisv->actor;

    while (actor->child != NULL) {
        Actor* nextActor = actor->child;

        if (nextActor->id == ACTOR_EN_BB) {
            nextActor->parent = NULL;
            actor->child = NULL;
            nextActor->params = ENBB_KILL_TRAIL;
        }
        actor = nextActor;
    }
    thisv->actor.child = NULL;
}

void EnBb_Init(Actor* thisx, GlobalContext* globalCtx) {
    EffectBlureInit1 blureInit;
    s32 pad;
    EnBb* thisv = (EnBb*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_Bb_Skel_001A30, &object_Bb_Anim_000444, thisv->jointTable,
                   thisv->morphTable, 16);
    thisv->unk_254 = 0;
    thisx->colChkInfo.health = 4;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, thisx, &sJntSphInit, thisv->elements);

    thisv->actionState = thisx->params >> 8;

    if (thisx->params & 0x80) {
        thisx->params |= 0xFF00;
    }
    if (thisx->params <= ENBB_BLUE) {
        ActorShape_Init(&thisx->shape, 200.0f, ActorShadow_DrawCircle, 35.0f);
    }
    if (thisx->params & 0xFF00) {
        thisv->timer = 0;
        thisv->flameScaleY = 80.0f;
        thisv->flameScaleX = 100.0f;
        thisv->collider.elements[0].info.toucherFlags = TOUCH_ON | TOUCH_SFX_HARD;
        thisv->collider.elements[0].info.toucher.dmgFlags = 0xFFCFFFFF;
        thisv->collider.elements[0].info.toucher.damage = 8;
        thisv->bobSize = thisv->actionState * 20.0f;
        thisv->flamePrimAlpha = 255;
        thisv->moveMode = BBMOVE_NORMAL;
        Actor_SetScale(thisx, 0.01f);
        switch (thisx->params) {
            case ENBB_BLUE:
                thisx->naviEnemyId = 0x1C;
                thisx->colChkInfo.damageTable = &sDamageTableBlueGreen;
                thisv->flamePrimBlue = thisv->flameEnvColor.b = 255;
                thisx->world.pos.y += 50.0f;
                EnBb_SetupBlue(thisv);
                thisx->flags |= ACTOR_FLAG_14;
                break;
            case ENBB_RED:
                thisx->naviEnemyId = 0x24;
                thisx->colChkInfo.damageTable = &sDamageTableRed;
                thisv->flameEnvColor.r = 255;
                thisv->collider.elements[0].info.toucher.effect = 1;
                EnBb_SetupRed(globalCtx, thisv);
                break;
            case ENBB_WHITE:
                thisx->naviEnemyId = 0x1D;
                thisx->colChkInfo.damageTable = &sDamageTableWhite;
                thisv->path = thisv->actionState;
                blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] =
                    blureInit.p1StartColor[3] = blureInit.p2StartColor[0] = blureInit.p2StartColor[1] =
                        blureInit.p2StartColor[2] = blureInit.p2StartColor[3] = blureInit.p1EndColor[0] =
                            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] =
                                blureInit.p2EndColor[1] = blureInit.p2EndColor[2] = 255;

                blureInit.p1EndColor[3] = 0;
                blureInit.p2EndColor[3] = 0;
                blureInit.elemDuration = 16;
                blureInit.unkFlag = 0;
                blureInit.calcMode = 2;

                Effect_Add(globalCtx, &thisv->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
                EnBb_SetupWhite(globalCtx, thisv);
                EnBb_SetWaypoint(thisv, globalCtx);
                EnBb_FaceWaypoint(thisv);
                thisx->flags |= ACTOR_FLAG_14;
                break;
            case ENBB_GREEN_BIG:
                thisv->path = thisv->actionState >> 4;
                thisv->collider.elements[0].dim.modelSphere.radius = 0x16;
                Actor_SetScale(thisx, 0.03f);
            case ENBB_GREEN:
                thisx->naviEnemyId = 0x1E;
                thisv->bobSize = (thisv->actionState & 0xF) * 20.0f;
                thisx->colChkInfo.damageTable = &sDamageTableBlueGreen;
                thisv->flameEnvColor.g = 255;
                thisx->colChkInfo.health = 1;

                EnBb_InitGreen(thisv, globalCtx);
                break;
        }
        thisx->focus.pos = thisx->world.pos;
    } else {
        EnBb_SetupFlameTrail(thisv);
    }
    thisv->collider.elements[0].dim.worldSphere.radius =
        thisv->collider.elements[0].dim.modelSphere.radius * thisv->collider.elements[0].dim.scale;
}

void EnBb_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnBb* thisv = (EnBb*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnBb_SetupFlameTrail(EnBb* thisv) {
    thisv->action = BB_FLAME_TRAIL;
    thisv->moveMode = BBMOVE_NOCLIP;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    EnBb_SetupAction(thisv, EnBb_FlameTrail);
}

void EnBb_FlameTrail(EnBb* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params == ENBB_KILL_TRAIL) {
        if (thisv->actor.parent == NULL) {
            EnBb_SetupDeath(thisv, globalCtx);
        }
    } else {
        if (thisv->timer == 0) {
            if (((EnBb*)thisv->targetActor)->flameScaleY != 0.0f) {
                Math_SmoothStepToF(&thisv->flameScaleY, thisv->actor.scale.y, 1.0f, thisv->actor.scale.y * 0.1f, 0.0f);
                Math_SmoothStepToF(&thisv->flameScaleX, thisv->actor.scale.z, 1.0f, thisv->actor.scale.z * 0.1f, 0.0f);
                if (thisv->flamePrimAlpha != thisv->vTrailMaxAlpha) {
                    thisv->flamePrimAlpha += 10;
                    if (thisv->vTrailMaxAlpha < thisv->flamePrimAlpha) {
                        thisv->flamePrimAlpha = thisv->vTrailMaxAlpha;
                    }
                }
            } else {
                if (!thisv->flamePrimAlpha) {
                    Actor_Kill(&thisv->actor);
                    return;
                } else if (thisv->flamePrimAlpha <= 20) {
                    thisv->flamePrimAlpha = 0;
                } else {
                    thisv->flamePrimAlpha -= 20;
                }
            }
            thisv->actor.world.pos = thisv->actor.parent->prevPos;
        } else {
            thisv->timer--;
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.parent->world.rot.y;
        }
    }
    if (thisv->actor.parent != NULL) {
        thisv->actor.velocity.y = thisv->actor.parent->velocity.y;
    }
}

void EnBb_SetupDeath(EnBb* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params <= ENBB_BLUE) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.speedXZ = -7.0f;
        thisv->timer = 5;
        thisv->actor.shape.rot.x += 0x4E20;
        EffectSsDeadSound_SpawnStationary(globalCtx, &thisv->actor.projectedPos, NA_SE_EN_BUBLE_DEAD, 1, 1, 0x28);
    }
    thisv->action = BB_KILL;
    EnBb_SetupAction(thisv, EnBb_Death);
}

void EnBb_Death(EnBb* thisv, GlobalContext* globalCtx) {
    s16 enpartType = 3;
    Vec3f sp40 = { 0.0f, 0.5f, 0.0f };
    Vec3f sp34 = { 0.0f, 0.0f, 0.0f };

    if (thisv->actor.params <= ENBB_BLUE) {
        Math_SmoothStepToF(&thisv->flameScaleY, 0.0f, 1.0f, 30.0f, 0.0f);
        Math_SmoothStepToF(&thisv->flameScaleX, 0.0f, 1.0f, 30.0f, 0.0f);
        if (thisv->timer != 0) {
            thisv->timer--;
            thisv->actor.shape.rot.x -= 0x4E20;
            return;
        }

        if (thisv->bodyBreak.val == BODYBREAK_STATUS_FINISHED) {
            BodyBreak_Alloc(&thisv->bodyBreak, 12, globalCtx);
        }

        if ((thisv->dmgEffect == 7) || (thisv->dmgEffect == 5)) {
            enpartType = 11;
        }

        if (!BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, enpartType)) {
            return;
        }
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xD0);
    } else {
        if (thisv->flamePrimAlpha) {
            if (thisv->flamePrimAlpha <= 20) {
                thisv->flamePrimAlpha = 0;
            } else {
                thisv->flamePrimAlpha -= 20;
            }
            return;
        }
    }
    Actor_Kill(&thisv->actor);
}

void EnBb_SetupDamage(EnBb* thisv) {
    thisv->action = BB_DAMAGE;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_DAMAGE);
    if (thisv->actor.params > ENBB_GREEN) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        if ((thisv->actor.bgCheckFlags & 8) == 0) {
            thisv->actor.speedXZ = -7.0f;
        }
        thisv->actor.shape.yOffset = 1500.0f;
    }
    if (thisv->actor.params == ENBB_RED) {
        EnBb_KillFlameTrail(thisv);
    }
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0xC);
    thisv->timer = 5;
    EnBb_SetupAction(thisv, EnBb_Damage);
}

void EnBb_Damage(EnBb* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (thisv->actor.speedXZ == 0.0f) {
        thisv->actor.shape.yOffset = 200.0f;
        EnBb_SetupDown(thisv);
    }
}

void EnBb_SetupBlue(EnBb* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000444);
    thisv->actor.speedXZ = (Rand_ZeroOne() * 0.5f) + 0.5f;
    thisv->timer = (Rand_ZeroOne() * 20.0f) + 40.0f;
    thisv->unk_264 = (Rand_ZeroOne() * 30.0f) + 180.0f;
    thisv->targetActor = NULL;
    thisv->action = BB_BLUE;
    EnBb_SetupAction(thisv, EnBb_Blue);
}

void EnBb_Blue(EnBb* thisv, GlobalContext* globalCtx) {
    Actor* explosive;
    s16 moveYawToWall;
    s16 thisYawToWall;
    s16 afterHitAngle;

    Math_SmoothStepToF(&thisv->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
    Math_SmoothStepToF(&thisv->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
    if (thisv->actor.floorHeight > BGCHECK_Y_MIN) {
        Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight + 50.0f + thisv->flyHeightMod, 1.0f, 0.5f,
                           0.0f);
    }
    SkelAnime_Update(&thisv->skelAnime);
    if (Math_CosF(thisv->bobPhase) == 0.0f) {
        if (thisv->charge) {
            thisv->bobSpeedMod = Rand_ZeroOne() * 2.0f;
        } else {
            thisv->bobSpeedMod = Rand_ZeroOne() * 4.0f;
        }
    }
    thisv->actor.world.pos.y += Math_CosF(thisv->bobPhase) * (1.0f + thisv->bobSpeedMod);
    thisv->bobPhase += 0.2f;
    Math_SmoothStepToF(&thisv->actor.speedXZ, thisv->maxSpeed, 1.0f, 0.5f, 0.0f);

    if (Math_Vec3f_DistXZ(&thisv->actor.world.pos, &thisv->actor.home.pos) > 300.0f) {
        thisv->vMoveAngleY = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->vMoveAngleY, 1, 0x7D0, 0);
    } else {
        thisv->timer--;
        if (thisv->timer <= 0) {
            thisv->charge ^= true;
            thisv->flyHeightMod = (s16)(Math_CosF(thisv->bobPhase) * 10.0f);
            thisv->actor.speedXZ = 0.0f;
            if (thisv->charge && (thisv->targetActor == NULL)) {
                thisv->vMoveAngleY = thisv->actor.world.rot.y;
                if (thisv->actor.xzDistToPlayer < 200.0f) {
                    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000184);
                    thisv->vMoveAngleY = thisv->actor.yawTowardsPlayer;
                }
                thisv->maxSpeed = (Rand_ZeroOne() * 1.5f) + 6.0f;
                thisv->timer = (Rand_ZeroOne() * 5.0f) + 20.0f;
                thisv->actionState = BBBLUE_NORMAL;
            } else {
                Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000444);
                thisv->maxSpeed = (Rand_ZeroOne() * 1.5f) + 1.0f;
                thisv->timer = (Rand_ZeroOne() * 20.0f) + 40.0f;
                thisv->vMoveAngleY = Math_SinF(thisv->bobPhase) * 65535.0f;
            }
        }
        if ((thisv->actor.xzDistToPlayer < 150.0f) && (thisv->actionState != BBBLUE_NORMAL)) {
            if (!thisv->charge) {
                Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000184);
                thisv->maxSpeed = (Rand_ZeroOne() * 1.5f) + 6.0f;
                thisv->timer = (Rand_ZeroOne() * 5.0f) + 20.0f;
                thisv->vMoveAngleY = thisv->actor.yawTowardsPlayer;
                thisv->actionState = thisv->charge = true; // Sets actionState to BBBLUE_AGGRO
            }
        } else if (thisv->actor.xzDistToPlayer < 200.0f) {
            thisv->vMoveAngleY = thisv->actor.yawTowardsPlayer;
        }
        if (thisv->targetActor == NULL) {
            explosive = EnBb_FindExplosive(globalCtx, thisv, 300.0f);
        } else if (thisv->targetActor->params == 0) {
            explosive = thisv->targetActor;
        } else {
            explosive = NULL;
        }
        if (explosive != NULL) {
            thisv->vMoveAngleY = Actor_WorldYawTowardActor(&thisv->actor, explosive);
            if ((thisv->vBombHopPhase == 0) && (explosive != thisv->targetActor)) {
                thisv->vBombHopPhase = -0x8000;
                thisv->targetActor = explosive;
                thisv->actor.speedXZ *= 0.5f;
            }
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->vMoveAngleY, 1, 0x1388, 0);
            Math_SmoothStepToF(&thisv->actor.world.pos.x, explosive->world.pos.x, 1.0f, 1.5f, 0.0f);
            Math_SmoothStepToF(&thisv->actor.world.pos.y, explosive->world.pos.y + 40.0f, 1.0f, 1.5f, 0.0f);
            Math_SmoothStepToF(&thisv->actor.world.pos.z, explosive->world.pos.z, 1.0f, 1.5f, 0.0f);
        } else {
            thisv->targetActor = NULL;
        }
        if (thisv->vBombHopPhase != 0) {
            thisv->actor.world.pos.y += -Math_CosS(thisv->vBombHopPhase) * 10.0f;
            thisv->vBombHopPhase += 0x1000;
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->vMoveAngleY, 1, 0x7D0, 0);
        }
        thisYawToWall = thisv->actor.wallYaw - thisv->actor.world.rot.y;
        moveYawToWall = thisv->actor.wallYaw - thisv->vMoveAngleY;
        if ((thisv->targetActor == NULL) && (thisv->actor.bgCheckFlags & 8) &&
            (ABS(thisYawToWall) > 0x4000 || ABS(moveYawToWall) > 0x4000)) {
            thisv->vMoveAngleY = thisv->actor.wallYaw + thisv->actor.wallYaw - thisv->actor.world.rot.y - 0x8000;
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->vMoveAngleY, 1, 0xBB8, 0);
        }
    }
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->vMoveAngleY, 1, 0x3E8, 0);
    if ((thisv->collider.base.acFlags & AC_HIT) || (thisv->collider.base.atFlags & AT_HIT)) {
        thisv->vMoveAngleY = thisv->actor.yawTowardsPlayer + 0x8000;
        if (thisv->collider.base.acFlags & AC_HIT) {
            afterHitAngle = -0x8000;
        } else {
            afterHitAngle = 0x4000;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_BITE);
            if (globalCtx->gameplayFrames & 1) {
                afterHitAngle = -0x4000;
            }
        }
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer + afterHitAngle;
        thisv->collider.base.acFlags &= ~AC_HIT;
        thisv->collider.base.atFlags &= ~AT_HIT;
    }

    if (thisv->maxSpeed >= 6.0f) {
        if ((s32)thisv->skelAnime.curFrame == 0 || (s32)thisv->skelAnime.curFrame == 5) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_MOUTH);
        } else if ((s32)thisv->skelAnime.curFrame == 2 || (s32)thisv->skelAnime.curFrame == 7) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_WING);
        }
    } else {
        if ((s32)thisv->skelAnime.curFrame == 5) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_WING);
        }
    }
    if (((s32)thisv->skelAnime.curFrame == 0) && (Rand_ZeroOne() < 0.1f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_LAUGH);
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnBb_SetupDown(EnBb* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000444);
    thisv->action = BB_DOWN;
    thisv->timer = 200;
    thisv->actor.colorFilterTimer = 0;
    thisv->actor.bgCheckFlags &= ~1;
    thisv->actor.speedXZ = 3.0f;
    thisv->flameScaleX = 0.0f;
    thisv->flameScaleY = 0.0f;
    thisv->actor.gravity = -2.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_DOWN);
    EnBb_SetupAction(thisv, EnBb_Down);
}

void EnBb_Down(EnBb* thisv, GlobalContext* globalCtx) {
    s16 yawDiff = thisv->actor.world.rot.y - thisv->actor.wallYaw;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.bgCheckFlags & 8) {
        if (ABS(yawDiff) > 0x4000) {
            thisv->actor.world.rot.y = thisv->actor.wallYaw + thisv->actor.wallYaw - thisv->actor.world.rot.y - 0x8000;
        }
        thisv->actor.bgCheckFlags &= ~8;
    }
    if (thisv->actor.bgCheckFlags & 3) {
        if (thisv->actor.params == ENBB_RED) {
            s32 floorType = func_80041D4C(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);

            if ((floorType == 2) || (floorType == 3) || (floorType == 9)) {
                thisv->moveMode = BBMOVE_HIDDEN;
                thisv->timer = 10;
                thisv->actionState++;
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                thisv->action = BB_RED;
                EnBb_SetupAction(thisv, EnBb_Red);
                return;
            }
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        if (thisv->actor.velocity.y < -14.0f) {
            thisv->actor.velocity.y *= -0.7f;
        } else {
            thisv->actor.velocity.y = 10.0f;
        }
        thisv->actor.bgCheckFlags &= ~1;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, 0);
        Math_SmoothStepToS(&thisv->actor.world.rot.y, -thisv->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if ((s32)thisv->skelAnime.curFrame == 5) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_WING);
    }
    if (thisv->timer == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_UP);
        switch (thisv->actor.params) {
            case ENBB_BLUE:
                thisv->actor.velocity.y = 0.0f;
                thisv->actor.gravity = 0.0f;
                EnBb_SetupBlue(thisv);
                break;
            case ENBB_RED:
                if (thisv->actor.velocity.y == 10.0f) {
                    EnBb_SetupRed(globalCtx, thisv);
                    EnBb_SpawnFlameTrail(globalCtx, thisv, true);
                }
                break;
            case ENBB_WHITE:
                thisv->actor.velocity.y = 0.0f;
                thisv->actor.gravity = 0.0f;
                EnBb_SetupWhite(globalCtx, thisv);
                thisv->actor.world.pos.y -= 60.0f;
                break;
        }
    } else {
        thisv->timer--;
    }
}

void EnBb_SetupRed(GlobalContext* globalCtx, EnBb* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000184);
    if (thisv->action == BB_DOWN) {
        thisv->actor.speedXZ = 5.0f;
        thisv->actor.gravity = -1.0f;
        thisv->actor.velocity.y = 16.0f;
        thisv->actionState = BBRED_ATTACK;
        thisv->timer = 0;
        thisv->moveMode = BBMOVE_NORMAL;
        thisv->actor.bgCheckFlags &= ~1;
    } else {
        thisv->actor.colChkInfo.health = 4;
        thisv->timer = 0;
        thisv->actionState = BBRED_WAIT;
        thisv->moveMode = BBMOVE_HIDDEN;
        thisv->actor.world.pos.y -= 80.0f;
        thisv->actor.home.pos = thisv->actor.world.pos;
        thisv->actor.velocity.y = thisv->actor.gravity = thisv->actor.speedXZ = 0.0f;
        thisv->actor.bgCheckFlags &= ~1;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    }
    thisv->action = BB_RED;
    EnBb_SetupAction(thisv, EnBb_Red);
}

void EnBb_Red(EnBb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 floorType;
    s16 yawDiff;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    switch (thisv->actionState) {
        case BBRED_WAIT:
            if ((Actor_WorldDistXYZToActor(&thisv->actor, &player->actor) <= 250.0f) && (ABS(yawDiff) <= 0x4000) &&
                (thisv->timer == 0)) {
                thisv->actor.speedXZ = 5.0f;
                thisv->actor.gravity = -1.0f;
                thisv->actor.velocity.y = 18.0f;
                thisv->moveMode = BBMOVE_NOCLIP;
                thisv->timer = 7;
                thisv->actor.bgCheckFlags &= ~1;
                thisv->actionState++;
                EnBb_SpawnFlameTrail(globalCtx, thisv, false);
            }
            break;
        case BBRED_ATTACK:
            if (thisv->timer == 0) {
                thisv->moveMode = BBMOVE_NORMAL;
                thisv->actor.flags |= ACTOR_FLAG_0;
            }
            thisv->bobPhase += Rand_ZeroOne();
            Math_SmoothStepToF(&thisv->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
            Math_SmoothStepToF(&thisv->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
            if (thisv->actor.bgCheckFlags & 8) {
                yawDiff = thisv->actor.world.rot.y - thisv->actor.wallYaw;
                if (ABS(yawDiff) > 0x4000) {
                    thisv->actor.world.rot.y =
                        thisv->actor.wallYaw + thisv->actor.wallYaw - thisv->actor.world.rot.y - 0x8000;
                }
                thisv->actor.bgCheckFlags &= ~8;
            }
            if (thisv->actor.bgCheckFlags & 1) {
                floorType = func_80041D4C(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
                if ((floorType == 2) || (floorType == 3) || (floorType == 9)) {
                    thisv->moveMode = BBMOVE_HIDDEN;
                    thisv->timer = 10;
                    thisv->actionState++;
                    thisv->actor.flags &= ~ACTOR_FLAG_0;
                } else {
                    thisv->actor.velocity.y *= -1.06f;
                    if (thisv->actor.velocity.y > 13.0f) {
                        thisv->actor.velocity.y = 13.0f;
                    }
                    thisv->actor.world.rot.y = Math_SinF(thisv->bobPhase) * 65535.0f;
                }
                thisv->actor.bgCheckFlags &= ~1;
            }
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
            if (Actor_GetCollidedExplosive(globalCtx, &thisv->collider.base) != NULL) {
                EnBb_SetupDown(thisv);
            }
            break;
        case BBRED_HIDE:
            if (thisv->timer == 0) {
                thisv->actor.speedXZ = 0.0f;
                thisv->actor.gravity = 0.0f;
                thisv->actor.velocity.y = 0.0f;
                thisv->actionState = BBRED_WAIT;
                thisv->timer = 120;
                thisv->actor.world.pos = thisv->actor.home.pos;
                thisv->actor.shape.rot = thisv->actor.world.rot = thisv->actor.home.rot;
                EnBb_KillFlameTrail(thisv);
            }
            break;
    }
    if (thisv->actionState != BBRED_WAIT) {
        if (((s32)thisv->skelAnime.curFrame == 0) || ((s32)thisv->skelAnime.curFrame == 5)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_MOUTH);
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLEFALL_FIRE - SFX_FLAG);
    }
}

void EnBb_FaceWaypoint(EnBb* thisv) {
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->waypointPos);
}

void EnBb_SetWaypoint(EnBb* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[thisv->path];
    Vec3s* point;

    if (thisv->waypoint == (s16)(path->count - 1)) {
        thisv->waypoint = 0;
    } else {
        thisv->waypoint++;
    }
    point = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->waypoint;
    thisv->waypointPos.x = point->x;
    thisv->waypointPos.y = point->y;
    thisv->waypointPos.z = point->z;
}

void EnBb_SetupWhite(GlobalContext* globalCtx, EnBb* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000444);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.pos.y += 60.0f;
    thisv->flameScaleX = 100.0f;
    thisv->action = BB_WHITE;
    thisv->waypoint = 0;
    thisv->timer = (Rand_ZeroOne() * 30.0f) + 40.0f;
    thisv->maxSpeed = 7.0f;
    EnBb_SetupAction(thisv, EnBb_White);
}

void EnBb_White(EnBb* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.speedXZ == 0.0f) {
        f32 distL1;
        f32 vx;
        f32 vz;
        s16 pitch = Math_Vec3f_Pitch(&thisv->actor.world.pos, &thisv->waypointPos);
        f32 vy = Math_SinS(pitch) * thisv->maxSpeed;
        f32 vxz = Math_CosS(pitch) * thisv->maxSpeed;

        vx = Math_SinS(thisv->actor.shape.rot.y) * vxz;
        vz = Math_CosS(thisv->actor.shape.rot.y) * vxz;
        distL1 = Math_SmoothStepToF(&thisv->actor.world.pos.x, thisv->waypointPos.x, 1.0f, ABS(vx), 0.0f);
        distL1 += Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->waypointPos.y, 1.0f, ABS(vy), 0.0f);
        distL1 += Math_SmoothStepToF(&thisv->actor.world.pos.z, thisv->waypointPos.z, 1.0f, ABS(vz), 0.0f);
        thisv->bobPhase += (0.05f + (Rand_ZeroOne() * 0.01f));
        if (distL1 == 0.0f) {
            thisv->timer--;
            if (thisv->timer == 0) {
                EnBb_SetWaypoint(thisv, globalCtx);
                EnBb_FaceWaypoint(thisv);
                Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000184);
                thisv->timer = Rand_ZeroOne() * 30.0f + 40.0f;
            } else {
                if (thisv->moveMode != BBMOVE_NORMAL) {
                    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000444);
                }
                thisv->actor.world.rot.y += 0x1F40;
            }
            thisv->moveMode = BBMOVE_NORMAL;
            thisv->maxSpeed = 0.0f;
        } else {
            thisv->moveMode = BBMOVE_NOCLIP;
            thisv->maxSpeed = 10.0f;
        }
        if (thisv->collider.base.atFlags & AT_HIT) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_BITE);
            thisv->collider.base.atFlags &= ~AT_HIT;
        }
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    } else if (Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f) == 0.0f) {
        EnBb_FaceWaypoint(thisv);
    }
    SkelAnime_Update(&thisv->skelAnime);
    if (((s32)thisv->skelAnime.curFrame == 0) && (Rand_ZeroOne() <= 0.1f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_LAUGH);
    }

    if ((thisv->maxSpeed != 0.0f) && (((s32)thisv->skelAnime.curFrame == 0) || ((s32)thisv->skelAnime.curFrame == 5))) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_MOUTH);
    } else if (((s32)thisv->skelAnime.curFrame == 2) || ((s32)thisv->skelAnime.curFrame == 7)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_WING);
    }
}

void EnBb_InitGreen(EnBb* thisv, GlobalContext* globalCtx) {
    Vec3f bobOffset = { 0.0f, 0.0f, 0.0f };

    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000444);
    thisv->moveMode = BBMOVE_NOCLIP;
    thisv->actionState = BBGREEN_FLAME_ON;
    thisv->bobPhase = Rand_ZeroOne();
    thisv->actor.shape.rot.x = thisv->actor.shape.rot.z = 0;
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    if (thisv->actor.params == ENBB_GREEN_BIG) {
        EnBb_SetWaypoint(thisv, globalCtx);
        EnBb_FaceWaypoint(thisv);
    }
    Matrix_Translate(thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->actor.world.rot.x, thisv->actor.world.rot.y, 0, MTXMODE_APPLY);
    Matrix_RotateZ(thisv->bobPhase, MTXMODE_APPLY);
    bobOffset.y = thisv->bobSize;
    Matrix_MultVec3f(&bobOffset, &thisv->actor.world.pos);
    thisv->targetActor = NULL;
    thisv->action = BB_GREEN;
    thisv->actor.speedXZ = 0.0f;
    thisv->vFlameTimer = (Rand_ZeroOne() * 30.0f) + 180.0f;
    EnBb_SetupAction(thisv, EnBb_Green);
}

void EnBb_SetupGreen(EnBb* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_Bb_Anim_000444);
    thisv->moveMode = BBMOVE_NOCLIP;
    thisv->actionState = BBGREEN_FLAME_ON;
    thisv->targetActor = NULL;
    thisv->action = BB_GREEN;
    thisv->actor.speedXZ = 0.0f;
    thisv->vFlameTimer = (Rand_ZeroOne() * 30.0f) + 180.0f;
    thisv->actor.shape.rot.z = 0;
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    EnBb_SetupAction(thisv, EnBb_Green);
}

void EnBb_Green(EnBb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f bobOffset = { 0.0f, 0.0f, 0.0f };
    Vec3f nextPos = player->actor.world.pos;

    nextPos.y += 30.0f;
    if (thisv->actor.params == ENBB_GREEN_BIG) {
        if (thisv->actor.speedXZ == 0.0f) {
            s16 pitch = Math_Vec3f_Pitch(&thisv->actor.home.pos, &thisv->waypointPos);
            s16 yaw = Math_Vec3f_Yaw(&thisv->actor.home.pos, &thisv->waypointPos);
            f32 vy = Math_SinS(pitch) * thisv->maxSpeed;
            f32 vxz = Math_CosS(pitch) * thisv->maxSpeed;
            f32 vz;
            f32 vx;
            f32 distL1;

            Math_SmoothStepToS(&thisv->actor.world.rot.y, yaw, 1, 0x3E8, 0);
            vx = Math_SinS(thisv->actor.world.rot.y) * vxz;
            distL1 = Math_CosS(thisv->actor.world.rot.y) * vxz;
            vz = Math_SmoothStepToF(&thisv->actor.home.pos.x, thisv->waypointPos.x, 1.0f, ABS(vx), 0.0f);
            vz += Math_SmoothStepToF(&thisv->actor.home.pos.y, thisv->waypointPos.y, 1.0f, ABS(vy), 0.0f);
            vz += Math_SmoothStepToF(&thisv->actor.home.pos.z, thisv->waypointPos.z, 1.0f, ABS(distL1), 0.0f);
            thisv->bobPhase += (0.05f + (Rand_ZeroOne() * 0.01f));
            if (vz == 0.0f) {
                EnBb_SetWaypoint(thisv, globalCtx);
            }
            thisv->moveMode = BBMOVE_NOCLIP;
            thisv->maxSpeed = 10.0f;
            if (thisv->collider.base.atFlags & AT_HIT) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_BITE);
                thisv->collider.base.atFlags &= ~AT_HIT;
            }
            if (Math_CosF(thisv->bobPhase) == 0.0f) {
                if (thisv->charge) {
                    thisv->bobSpeedMod = Rand_ZeroOne();
                } else {
                    thisv->bobSpeedMod = Rand_ZeroOne() * 3.0f;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_LAUGH);
                }
            }
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        } else if (Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f) == 0.0f) {
            EnBb_FaceWaypoint(thisv);
        }
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 0);
        Math_SmoothStepToS(&thisv->actor.shape.rot.x, Math_Vec3f_Pitch(&thisv->actor.world.pos, &nextPos), 1, 0xFA0, 0);
    }
    SkelAnime_Update(&thisv->skelAnime);
    if (Math_CosF(thisv->bobPhase) <= 0.002f) {
        thisv->bobSpeedMod = Rand_ZeroOne() * 0.05f;
    }
    Matrix_Translate(thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->actor.world.rot.x, thisv->actor.world.rot.y, 0, MTXMODE_APPLY);
    Matrix_RotateZ(thisv->bobPhase, MTXMODE_APPLY);
    bobOffset.y = thisv->bobSize;
    Matrix_MultVec3f(&bobOffset, &nextPos);
    Math_SmoothStepToF(&thisv->actor.world.pos.x, nextPos.x, 1.0f, thisv->bobPhase * 0.75f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.world.pos.y, nextPos.y, 1.0f, thisv->bobPhase * 0.75f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.world.pos.z, nextPos.z, 1.0f, thisv->bobPhase * 0.75f, 0.0f);
    thisv->bobPhase += 0.1f + thisv->bobSpeedMod;
    if (Actor_GetCollidedExplosive(globalCtx, &thisv->collider.base) || (--thisv->vFlameTimer == 0)) {
        thisv->actionState++;
        thisv->timer = (Rand_ZeroOne() * 30.0f) + 60.0f;
        if (thisv->vFlameTimer != 0) {
            thisv->collider.base.acFlags &= ~AC_HIT;
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_DOWN);
    }
    if (thisv->actionState != BBGREEN_FLAME_ON) {
        thisv->timer--;
        if (thisv->timer == 0) {
            thisv->actionState = BBGREEN_FLAME_ON;
            thisv->vFlameTimer = (Rand_ZeroOne() * 30.0f) + 180.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_UP);
        }
        Math_SmoothStepToF(&thisv->flameScaleY, 0.0f, 1.0f, 10.0f, 0.0f);
        Math_SmoothStepToF(&thisv->flameScaleX, 0.0f, 1.0f, 10.0f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
        Math_SmoothStepToF(&thisv->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
    }
    if ((s32)thisv->skelAnime.curFrame == 5) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_WING);
    }
    if (((s32)thisv->skelAnime.curFrame == 0) && (Rand_ZeroOne() < 0.1f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_LAUGH);
    }
}

void EnBb_SetupStunned(EnBb* thisv) {
    thisv->action = BB_STUNNED;
    if (thisv->actor.params != ENBB_WHITE) {
        if (thisv->actor.params != ENBB_RED) {
            if (thisv->actor.params > ENBB_GREEN) {
                thisv->actor.gravity = -2.0f;
                thisv->actor.shape.yOffset = 1500.0f;
            }
            thisv->actor.speedXZ = 0.0f;
            thisv->flameScaleX = 0.0f;
            thisv->flameScaleY = 0.0f;
        } else {
            EnBb_KillFlameTrail(thisv);
        }
    }
    switch (thisv->dmgEffect) {
        case 8:
            Actor_SetColorFilter(&thisv->actor, -0x8000, 0xC8, 0, 0x50);
            break;
        case 9:
            thisv->fireIceTimer = 0x30;
        case 15:
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
            Actor_SetColorFilter(&thisv->actor, 0, 0xB4, 0, 0x50);
            break;
    }
    thisv->actor.bgCheckFlags &= ~1;
    EnBb_SetupAction(thisv, EnBb_Stunned);
}

void EnBb_Stunned(EnBb* thisv, GlobalContext* globalCtx) {
    s16 yawDiff = thisv->actor.world.rot.y - thisv->actor.wallYaw;

    if (thisv->actor.bgCheckFlags & 8) {
        if (ABS(yawDiff) > 0x4000) {
            thisv->actor.world.rot.y = thisv->actor.wallYaw + thisv->actor.wallYaw - thisv->actor.world.rot.y - 0x8000;
        }
        thisv->actor.bgCheckFlags &= ~8;
    }
    if (thisv->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        if (thisv->actor.velocity.y < -14.0f) {
            thisv->actor.velocity.y *= -0.4f;
        } else {
            thisv->actor.velocity.y = 0.0f;
        }
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, 0);
    }
    if (thisv->actor.colorFilterTimer == 0) {
        thisv->actor.shape.yOffset = 200.0f;
        if (thisv->actor.colChkInfo.health != 0) {
            if ((thisv->actor.params == ENBB_GREEN) || (thisv->actor.params == ENBB_GREEN_BIG)) {
                EnBb_SetupGreen(thisv);
            } else if (thisv->actor.params == ENBB_WHITE) {
                thisv->action = BB_WHITE;
                EnBb_SetupAction(thisv, EnBb_White);
            } else {
                EnBb_SetupDown(thisv);
            }
        } else {
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            EnBb_SetupDeath(thisv, globalCtx);
        }
    }
}

void EnBb_CollisionCheck(EnBb* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.atFlags & AT_BOUNCED) {
        thisv->collider.base.atFlags &= ~AT_BOUNCED;
        if (thisv->action != BB_DOWN) {
            if (thisv->actor.params >= ENBB_RED) {
                thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer + 0x8000;
                if (thisv->actor.params == ENBB_RED) {
                    EnBb_KillFlameTrail(thisv);
                }
                EnBb_SetupDown(thisv);
                return;
            }
            thisv->actionVar2 = 1;
        }
    }
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        thisv->dmgEffect = thisv->actor.colChkInfo.damageEffect;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.elements[0].info, 0);
        switch (thisv->dmgEffect) {
            case 7:
                thisv->actor.freezeTimer = thisv->collider.elements[0].info.acHitInfo->toucher.damage;
            case 5:
                thisv->fireIceTimer = 0x30;
                //! @bug
                //! Setting fireIceTimer here without calling Actor_SetColorFilter causes a crash if the bubble is
                //! killed in a single hit by an attack with damage effect 5 or 7 while actor updating is halted. Using
                //! Din's Fire on a white bubble will do just that. The mechanism is complex and described below.
                goto block_15;
            case 6:
                thisv->actor.freezeTimer = thisv->collider.elements[0].info.acHitInfo->toucher.damage;
                break;
            case 8:
            case 9:
            case 15:
                if (thisv->action != BB_STUNNED) {
                    Actor_ApplyDamage(&thisv->actor);
                    EnBb_SetupStunned(thisv);
                }
                break;
            default:
            block_15:
                if ((thisv->dmgEffect == 14) || (thisv->dmgEffect == 12) || (thisv->dmgEffect == 11) ||
                    (thisv->dmgEffect == 10) || (thisv->dmgEffect == 7) || (thisv->dmgEffect == 5)) {
                    if ((thisv->action != BB_DOWN) || (thisv->timer < 190)) {
                        Actor_ApplyDamage(&thisv->actor);
                    }
                    if ((thisv->action != BB_DOWN) && (thisv->actor.params != ENBB_WHITE)) {
                        EnBb_SetupDown(thisv);
                    }
                } else {
                    if (((thisv->action == BB_DOWN) && (thisv->timer < 190)) ||
                        ((thisv->actor.params != ENBB_WHITE) && (thisv->flameScaleX < 20.0f))) {
                        Actor_ApplyDamage(&thisv->actor);
                    } else {
                        thisv->collider.base.acFlags |= AC_HIT;
                    }
                }
                if (thisv->actor.colChkInfo.health == 0) {
                    thisv->actor.flags &= ~ACTOR_FLAG_0;
                    if (thisv->actor.params == ENBB_RED) {
                        EnBb_KillFlameTrail(thisv);
                    }
                    EnBb_SetupDeath(thisv, globalCtx);
                    //! @bug
                    //! Because Din's Fire kills the bubble in a single hit, Actor_SetColorFilter is never called and
                    //! colorFilterParams is never set. And because Din's Fire halts updating during its cutscene,
                    //! EnBb_Death doesn't kill the bubble on the next frame like it should. This combines with
                    //! the bug in EnBb_Draw below to crash the game.
                } else if ((thisv->actor.params == ENBB_WHITE) &&
                           ((thisv->action == BB_WHITE) || (thisv->action == BB_STUNNED))) {
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0xC);
                    thisv->actor.speedXZ = -8.0f;
                    thisv->maxSpeed = 0.0f;
                    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_DAMAGE);
                } else if (((thisv->action == BB_DOWN) && (thisv->timer < 190)) ||
                           ((thisv->actor.params != ENBB_WHITE) && (thisv->flameScaleX < 20.0f))) {
                    EnBb_SetupDamage(thisv);
                }
            case 13:
                break;
        }
    }
}

void EnBb_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnBb* thisv = (EnBb*)thisx;
    Vec3f sp4C = { 0.0f, 0.0f, 0.0f };
    Vec3f sp40 = { 0.0f, -0.6f, 0.0f };
    Color_RGBA8 sp3C = { 0, 0, 255, 255 };
    Color_RGBA8 sp38 = { 0, 0, 0, 0 };
    f32 sp34 = -15.0f;

    if (thisv->actor.params <= ENBB_BLUE) {
        EnBb_CollisionCheck(thisv, globalCtx);
    }
    if (thisv->actor.colChkInfo.damageEffect != 0xD) {
        thisv->actionFunc(thisv, globalCtx);
        if ((thisv->actor.params <= ENBB_BLUE) && (thisv->actor.speedXZ >= -6.0f) &&
            ((thisv->actor.flags & ACTOR_FLAG_15) == 0)) {
            Actor_MoveForward(&thisv->actor);
        }
        if (thisv->moveMode == BBMOVE_NORMAL) {
            if ((thisv->actor.world.pos.y - 20.0f) <= thisv->actor.floorHeight) {
                sp34 = 20.0f;
            }
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, sp34, 25.0f, 20.0f, 5);
        }
        thisv->actor.focus.pos = thisv->actor.world.pos;
        thisv->collider.elements->dim.worldSphere.center.x = thisv->actor.world.pos.x;
        thisv->collider.elements->dim.worldSphere.center.y =
            thisv->actor.world.pos.y + (thisv->actor.shape.yOffset * thisv->actor.scale.y);
        thisv->collider.elements->dim.worldSphere.center.z = thisv->actor.world.pos.z;

        if ((thisv->action > BB_KILL) && ((thisv->actor.speedXZ != 0.0f) || (thisv->action == BB_GREEN))) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if ((thisv->action > BB_FLAME_TRAIL) &&
            ((thisv->actor.colorFilterTimer == 0) || !(thisv->actor.colorFilterParams & 0x4000)) &&
            (thisv->moveMode != BBMOVE_HIDDEN)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
}

void EnBb_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnBb* thisv = (EnBb*)thisx;

    BodyBreak_SetInfo(&thisv->bodyBreak, limbIndex, 4, 15, 15, dList, BODYBREAK_OBJECT_DEFAULT);
}

static Vec3f sFireIceOffsets[] = {
    { 13.0f, 10.0f, 0.0f }, { 5.0f, 25.0f, 5.0f },   { -5.0f, 25.0f, 5.0f },  { -13.0f, 10.0f, 0.0f },
    { 5.0f, 25.0f, -5.0f }, { -5.0f, 25.0f, -5.0f }, { 0.0f, 10.0f, -13.0f }, { 5.0f, 0.0f, 5.0f },
    { 5.0f, 0.0f, -5.0f },  { 0.0f, 10.0f, 13.0f },  { -5.0f, 0.0f, 5.0f },   { -5.0f, 0.0f, -5.0f },
};

void EnBb_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnBb* thisv = (EnBb*)thisx;
    Vec3f blureBase1 = { 0.0f, 5000.0f, 0.0f };
    Vec3f blureBase2 = { 0.0f, 2000.0f, 0.0f };
    Vec3f blureVtx1;
    Vec3f blureVtx2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bb.c", 2044);

    blureBase1.z = thisv->maxSpeed * 80.0f;
    blureBase2.z = thisv->maxSpeed * 80.0f;
    if (thisv->moveMode != BBMOVE_HIDDEN) {
        if (thisv->actor.params <= ENBB_BLUE) {
            func_80093D18(globalCtx->state.gfxCtx);
            SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, EnBb_PostLimbDraw,
                              thisv);

            if (thisv->fireIceTimer != 0) {
                thisv->actor.colorFilterTimer++;
                //! @bug:
                //! The purpose of thisv is to counteract Actor_UpdateAll decrementing colorFilterTimer. However,
                //! the above bugs mean unk_2A8 can be nonzero without damage effects ever having been set.
                //! This routine will then increment colorFilterTimer, and on the next frame Actor_Draw will try
                //! to draw the unset colorFilterParams. This causes a divide-by-zero error, crashing the game.
                if (1) {}
                thisv->fireIceTimer--;
                if ((thisv->fireIceTimer % 4) == 0) {
                    Vec3f sp70;
                    s32 index = thisv->fireIceTimer >> 2;

                    sp70.x = thisv->actor.world.pos.x + sFireIceOffsets[index].x;
                    sp70.y = thisv->actor.world.pos.y + sFireIceOffsets[index].y;
                    sp70.z = thisv->actor.world.pos.z + sFireIceOffsets[index].z;

                    if ((thisv->dmgEffect != 7) && (thisv->dmgEffect != 5)) {
                        EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &sp70, 0x96, 0x96, 0x96, 0xFA, 0xEB,
                                                       0xF5, 0xFF, 0.8f);
                    } else {
                        sp70.y -= 17.0f;
                        EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &sp70, 0x28, 1, 0, -1);
                    }
                }
            }
            Matrix_Translate(0.0f, thisv->flameScaleX * -40.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(0.0f, -40.0f, 0.0f, MTXMODE_APPLY);
        }
        if (thisv->actor.params != ENBB_WHITE) {
            func_80093D84(globalCtx->state.gfxCtx);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                        ((globalCtx->gameplayFrames + (thisv->flameScrollMod * 10)) *
                                         (-20 - (thisv->flameScrollMod * -2))) %
                                            0x200,
                                        0x20, 0x80));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, thisv->flamePrimBlue, thisv->flamePrimAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, thisv->flameEnvColor.r, thisv->flameEnvColor.g, thisv->flameEnvColor.b, 0);
            Matrix_RotateY(((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisv->actor.shape.rot.y + 0x8000)) *
                               (std::numbers::pi_v<float> / 0x8000),
                           MTXMODE_APPLY);
            Matrix_Scale(thisv->flameScaleX * 0.01f, thisv->flameScaleY * 0.01f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bb.c", 2106),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        } else {
            Matrix_MultVec3f(&blureBase1, &blureVtx1);
            Matrix_MultVec3f(&blureBase2, &blureVtx2);
            if ((thisv->maxSpeed != 0.0f) && (thisv->action == BB_WHITE) && !(globalCtx->gameplayFrames & 1) &&
                (thisv->actor.colChkInfo.health != 0)) {
                EffectBlure_AddVertex(Effect_GetByIndex(thisv->blureIdx), &blureVtx1, &blureVtx2);
            } else if (thisv->action != BB_WHITE) {
                EffectBlure_AddSpace(Effect_GetByIndex(thisv->blureIdx));
            }
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bb.c", 2127);
}
