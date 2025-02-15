/*
 * File: z_en_tubo_trap.c
 * Overlay: ovl_En_Tubo_Trap
 * Description: Flying pot enemy
 */

#include "z_en_tubo_trap.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnTuboTrap_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTuboTrap_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTuboTrap_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTuboTrap_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnTuboTrap_WaitForProximity(EnTuboTrap* thisv, GlobalContext* globalCtx);
void EnTuboTrap_Levitate(EnTuboTrap* thisv, GlobalContext* globalCtx);
void EnTuboTrap_Fly(EnTuboTrap* thisv, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 9, 23, 0, { 0 } },
};

ActorInit En_Tubo_Trap_InitVars = {
    ACTOR_EN_TUBO_TRAP,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(EnTuboTrap),
    (ActorFunc)EnTuboTrap_Init,
    (ActorFunc)EnTuboTrap_Destroy,
    (ActorFunc)EnTuboTrap_Update,
    (ActorFunc)EnTuboTrap_Draw,
    NULL,
};

void EnTuboTrap_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnTuboTrap* thisv = (EnTuboTrap*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 2.0f);
    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 壷トラップ ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.params); // "Urn Trap"
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    Actor_SetScale(&thisv->actor, 0.1f);
    thisv->actionFunc = EnTuboTrap_WaitForProximity;
}

void EnTuboTrap_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnTuboTrap* thisv = (EnTuboTrap*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnTuboTrap_DropCollectible(EnTuboTrap* thisv, GlobalContext* globalCtx) {
    s16 params = thisv->actor.params;
    s16 param3FF = (params >> 6) & 0x3FF;

    if (param3FF >= 0 && param3FF < 0x1A) {
        Item_DropCollectible(globalCtx, &thisv->actor.world.pos, param3FF | ((params & 0x3F) << 8));
    }
}

void EnTuboTrap_SpawnEffectsOnLand(EnTuboTrap* thisv, GlobalContext* globalCtx) {
    f32 rand;
    f32 cos;
    f32 sin;
    Vec3f pos;
    Vec3f velocity;
    s16 var;
    s32 arg5;
    s32 i;
    Vec3f* actorPos = &thisv->actor.world.pos;

    for (i = 0, var = 0; i < 15; i++, var += 20000) {
        sin = Math_SinS(var);
        cos = Math_CosS(var);
        pos.x = sin * 8.0f;
        pos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        pos.z = cos * 8.0f;

        velocity.x = pos.x * 0.23f;
        velocity.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        velocity.z = pos.z * 0.23f;

        pos.x += actorPos->x;
        pos.y += actorPos->y;
        pos.z += actorPos->z;

        rand = Rand_ZeroOne();
        if (rand < 0.2f) {
            arg5 = 96;
        } else if (rand < 0.6f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, actorPos, -240, arg5, 10, 10, 0,
                             (Rand_ZeroOne() * 65.0f) + 15.0f, 0, 32, 60, KAKERA_COLOR_NONE,
                             OBJECT_GAMEPLAY_DANGEON_KEEP, gPotFragmentDL);
    }

    func_80033480(globalCtx, actorPos, 30.0f, 4, 20, 50, 0);
}

void EnTuboTrap_SpawnEffectsInWater(EnTuboTrap* thisv, GlobalContext* globalCtx) {
    f32 rand;
    f32 cos;
    f32 sin;
    Vec3f pos;
    Vec3f velocity;
    s16 var;
    s32 arg5;
    s32 i;
    Vec3f* actorPos = &thisv->actor.world.pos;

    pos = *actorPos;
    pos.y += thisv->actor.yDistToWater;

    EffectSsGSplash_Spawn(globalCtx, &pos, 0, 0, 0, 400);

    for (i = 0, var = 0; i < 15; i++, var += 20000) {
        sin = Math_SinS(var);
        cos = Math_CosS(var);
        pos.x = sin * 8.0f;
        pos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        pos.z = cos * 8.0f;

        velocity.x = pos.x * 0.20f;
        velocity.y = (Rand_ZeroOne() * 4.0f) + 2.0f;
        velocity.z = pos.z * 0.20f;

        pos.x += actorPos->x;
        pos.y += actorPos->y;
        pos.z += actorPos->z;

        rand = Rand_ZeroOne();
        if (rand < 0.2f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, actorPos, -180, arg5, 30, 30, 0,
                             (Rand_ZeroOne() * 65.0f) + 15.0f, 0, 32, 70, KAKERA_COLOR_NONE,
                             OBJECT_GAMEPLAY_DANGEON_KEEP, gPotFragmentDL);
    }
}

void EnTuboTrap_HandleImpact(EnTuboTrap* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Player* player2 = GET_PLAYER(globalCtx);

    if ((thisv->actor.bgCheckFlags & 0x20) && (thisv->actor.yDistToWater > 15.0f)) {
        EnTuboTrap_SpawnEffectsInWater(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_BOMB_DROP_WATER);
        EnTuboTrap_DropCollectible(thisv, globalCtx);
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->collider.base.atFlags & AT_BOUNCED) {
        thisv->collider.base.atFlags &= ~AT_BOUNCED;
        EnTuboTrap_SpawnEffectsOnLand(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_IT_SHIELD_REFLECT_SW);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
        EnTuboTrap_DropCollectible(thisv, globalCtx);
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        EnTuboTrap_SpawnEffectsOnLand(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_EXPLOSION);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
        EnTuboTrap_DropCollectible(thisv, globalCtx);
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        if (thisv->collider.base.at == &player->actor) {
            EnTuboTrap_SpawnEffectsOnLand(thisv, globalCtx);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &player2->actor.world.pos, 40, NA_SE_PL_BODY_HIT);
            EnTuboTrap_DropCollectible(thisv, globalCtx);
            Actor_Kill(&thisv->actor);
            return;
        }
    }

    if ((thisv->actor.bgCheckFlags & 8) || (thisv->actor.bgCheckFlags & 1)) {
        EnTuboTrap_SpawnEffectsOnLand(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
        EnTuboTrap_DropCollectible(thisv, globalCtx);
        Actor_Kill(&thisv->actor);
        return;
    }
}

void EnTuboTrap_WaitForProximity(EnTuboTrap* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 targetHeight;

    if (BREG(2) != 0) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ わて     ☆☆☆☆☆ %f\n" VT_RST, thisv->actor.world.pos.y);   // "You"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ おいどん ☆☆☆☆☆ %f\n" VT_RST, player->actor.world.pos.y); // "Me"
        osSyncPrintf("\n\n");
    }

    if (thisv->actor.xzDistToPlayer < 200.0f && thisv->actor.world.pos.y <= player->actor.world.pos.y) {
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
        thisv->actor.flags |= ACTOR_FLAG_0;
        targetHeight = 40.0f + -10.0f * gSaveContext.linkAge;

        thisv->targetY = player->actor.world.pos.y + targetHeight;
        if (thisv->targetY < thisv->actor.world.pos.y) {
            thisv->targetY = thisv->actor.world.pos.y + targetHeight;
        }

        thisv->originPos = thisv->actor.world.pos;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_POT_MOVE_START);
        thisv->actionFunc = EnTuboTrap_Levitate;
    }
}

void EnTuboTrap_Levitate(EnTuboTrap* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.rot.y += 5000;
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->targetY, 0.8f, 3.0f);

    if (fabsf(thisv->actor.world.pos.y - thisv->targetY) < 10.0f) {
        thisv->actor.speedXZ = 10.0f;
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actionFunc = EnTuboTrap_Fly;
    }
}

void EnTuboTrap_Fly(EnTuboTrap* thisv, GlobalContext* globalCtx) {
    f32 dx = thisv->originPos.x - thisv->actor.world.pos.x;
    f32 dy = thisv->originPos.y - thisv->actor.world.pos.y;
    f32 dz = thisv->originPos.z - thisv->actor.world.pos.z;

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TUBOOCK_FLY - SFX_FLAG);

    if (240.0f < sqrtf(SQ(dx) + SQ(dy) + SQ(dz))) {
        Math_ApproachF(&thisv->actor.gravity, -3.0f, 0.2f, 0.5f);
    }

    thisv->actor.shape.rot.y += 5000;
    EnTuboTrap_HandleImpact(thisv, globalCtx);
}

void EnTuboTrap_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnTuboTrap* thisv = (EnTuboTrap*)thisx;
    s32 pad;

    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 20.0f, 0x1D);
    Actor_SetFocus(&thisv->actor, 0.0f);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnTuboTrap_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gPotDL);
}
