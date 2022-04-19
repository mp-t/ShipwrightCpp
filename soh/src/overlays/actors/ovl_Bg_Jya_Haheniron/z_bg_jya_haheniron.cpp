/*
 * File: z_bg_jya_haheniron
 * Overlay: ovl_Bg_Jya_Haheniron
 * Description: Chunks of Iron Knucle Chair and Pillar
 */

#include "z_bg_jya_haheniron.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_jya_iron/object_jya_iron.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaHaheniron_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaHaheniron_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaHaheniron_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaHaheniron_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgJyaHaheniron_SetupChairCrumble(BgJyaHaheniron* thisv);
void BgJyaHaheniron_ChairCrumble(BgJyaHaheniron* thisv, GlobalContext* globalCtx);
void BgJyaHaheniron_SetupPillarCrumble(BgJyaHaheniron* thisv);
void BgJyaHaheniron_PillarCrumble(BgJyaHaheniron* thisv, GlobalContext* globalCtx);
void BgJyaHaheniron_SetupRubbleCollide(BgJyaHaheniron* thisv);
void BgJyaHaheniron_RubbleCollide(BgJyaHaheniron* thisv, GlobalContext* globalCtx);

ActorInit Bg_Jya_Haheniron_InitVars = {
    ACTOR_BG_JYA_HAHENIRON,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_JYA_IRON,
    sizeof(BgJyaHaheniron),
    (ActorFunc)BgJyaHaheniron_Init,
    (ActorFunc)BgJyaHaheniron_Destroy,
    (ActorFunc)BgJyaHaheniron_Update,
    (ActorFunc)BgJyaHaheniron_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static s16 sKakeraScales[] = { 5, 8, 11, 14, 17 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE), ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static f32 D_80898794[] = { 0.13f, 0.1f, 0.1f };

static Vec3f D_808987A0[] = { 0.0f, 14.0f, 0.0f };

static Vec3f D_808987AC[] = { 0.0f, 8.0f, 0.0f };

void BgJyaHaheniron_ColliderInit(BgJyaHaheniron* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItems);
}

void BgJyaHaheniron_SpawnFragments(GlobalContext* globalCtx, Vec3f* vec1, Vec3f* vec2) {
    Vec3f vel;
    Vec3f pos;
    s16 arg5;
    s32 angle;
    s32 i;
    f32 rand1;

    for (angle = 0, i = 0; i < ARRAY_COUNT(sKakeraScales); i++) {
        rand1 = Rand_ZeroOne() * 10.0f;
        vel.x = (Math_SinS(angle) * rand1) + vec2->x;
        vel.y = (Rand_ZeroOne() * 10.0f) + vec2->y;
        vel.z = (Math_CosS(angle) * rand1) + vec2->z;

        rand1 = Rand_ZeroOne();
        if (rand1 < 0.2f) {
            arg5 = 96;
        } else if (rand1 < 0.8f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        EffectSsKakera_Spawn(globalCtx, vec1, &vel, vec1, -350, arg5, 40, 4, 0, sKakeraScales[i], 0, 20, 40,
                             KAKERA_COLOR_NONE, OBJECT_JYA_IRON, gObjectJyaIronDL_000880);
        angle += 0x3333;
    }
    pos.x = vec1->x + (vec2->x * 5.0f);
    pos.y = vec1->y + (vec2->y * 5.0f);
    pos.z = vec1->z + (vec2->z * 5.0f);
    func_80033480(globalCtx, &pos, 100.0f, 4, 100, 160, 1);
}

void BgJyaHaheniron_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgJyaHaheniron* thisv = (BgJyaHaheniron*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, D_80898794[thisv->actor.params]);
    if (thisv->actor.params == 0) {
        BgJyaHaheniron_ColliderInit(thisv, globalCtx);
        thisv->actor.shape.rot.z = (Rand_ZeroOne() * 65535.0f);
        BgJyaHaheniron_SetupChairCrumble(thisv);
    } else if (thisv->actor.params == 1) {
        BgJyaHaheniron_SetupPillarCrumble(thisv);
    } else if (thisv->actor.params == 2) {
        BgJyaHaheniron_SetupRubbleCollide(thisv);
    }
}

void BgJyaHaheniron_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgJyaHaheniron* thisv = (BgJyaHaheniron*)thisx;

    if (thisv->actor.params == 0) {
        Collider_DestroyJntSph(globalCtx, &thisv->collider);
    }
}

void BgJyaHaheniron_SetupChairCrumble(BgJyaHaheniron* thisv) {
    thisv->actionFunc = BgJyaHaheniron_ChairCrumble;
}

void BgJyaHaheniron_ChairCrumble(BgJyaHaheniron* thisv, GlobalContext* globalCtx) {
    Vec3f vec;

    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 8.0f, 0.0f, 0x85);
    if ((thisv->actor.bgCheckFlags & 9) || ((thisv->collider.base.atFlags & AT_HIT) && (thisv->collider.base.at != NULL) &&
                                           (thisv->collider.base.at->category == ACTORCAT_PLAYER))) {
        vec.x = -Rand_ZeroOne() * thisv->actor.velocity.x;
        vec.y = -Rand_ZeroOne() * thisv->actor.velocity.y;
        vec.z = -Rand_ZeroOne() * thisv->actor.velocity.z;
        BgJyaHaheniron_SpawnFragments(globalCtx, &thisv->actor.world.pos, &vec);
        Actor_Kill(&thisv->actor);
    } else if (thisv->timer > 60) {
        Actor_Kill(&thisv->actor);
    } else {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    thisv->actor.shape.rot.y += 0x4B0;
    thisv->actor.shape.rot.x += 0xFA0;
}

void BgJyaHaheniron_SetupPillarCrumble(BgJyaHaheniron* thisv) {
    thisv->actionFunc = BgJyaHaheniron_PillarCrumble;
}

void BgJyaHaheniron_PillarCrumble(BgJyaHaheniron* thisv, GlobalContext* globalCtx) {
    if (thisv->timer >= 8) {
        Actor_MoveForward(&thisv->actor);
    } else if (thisv->timer >= 17) {
        BgJyaHaheniron_SpawnFragments(globalCtx, &thisv->actor.world.pos, D_808987A0);
        Actor_Kill(&thisv->actor);
    }
    thisv->actor.shape.rot.y += 0x258;
    thisv->actor.shape.rot.x += 0x3E8;
}

void BgJyaHaheniron_SetupRubbleCollide(BgJyaHaheniron* thisv) {
    thisv->actionFunc = BgJyaHaheniron_RubbleCollide;
}

void BgJyaHaheniron_RubbleCollide(BgJyaHaheniron* thisv, GlobalContext* globalCtx) {
    if (thisv->timer >= 17) {
        BgJyaHaheniron_SpawnFragments(globalCtx, &thisv->actor.world.pos, D_808987AC);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 80, NA_SE_EN_IRONNACK_BREAK_PILLAR2);
        Actor_Kill(&thisv->actor);
    }
}

void BgJyaHaheniron_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgJyaHaheniron* thisv = (BgJyaHaheniron*)thisx;

    thisv->timer++;
    thisv->actionFunc(thisv, globalCtx);
}

void BgJyaHaheniron_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const Gfx* dLists[] = {
        gObjectJyaIronDL_000880,
        gObjectJyaIronDL_000AE0,
        gObjectJyaIronDL_000600,
    };
    s32 pad;
    BgJyaHaheniron* thisv = (BgJyaHaheniron*)thisx;

    if (thisv->actor.params == 0) {
        Collider_UpdateSpheres(0, &thisv->collider);
    }
    Gfx_DrawDListOpa(globalCtx, dLists[thisv->actor.params]);
}
