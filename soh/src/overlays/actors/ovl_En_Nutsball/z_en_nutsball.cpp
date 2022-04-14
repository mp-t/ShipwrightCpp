/*
 * File: z_en_nutsball.c
 * Overlay: ovl_En_Nutsball
 * Description: Projectile fired by deku scrubs
 */

#include "z_en_nutsball.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_dekunuts/object_dekunuts.h"
#include "objects/object_hintnuts/object_hintnuts.h"
#include "objects/object_shopnuts/object_shopnuts.h"
#include "objects/object_dns/object_dns.h"
#include "objects/object_dnk/object_dnk.h"

#define FLAGS ACTOR_FLAG_4

void EnNutsball_Init(Actor* thisx, GlobalContext* globalCtx);
void EnNutsball_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnNutsball_Update(Actor* thisx, GlobalContext* globalCtx);
void EnNutsball_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80ABBB34(EnNutsball* thisv, GlobalContext* globalCtx);
void func_80ABBBA8(EnNutsball* thisv, GlobalContext* globalCtx);

const ActorInit En_Nutsball_InitVars = {
    ACTOR_EN_NUTSBALL,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnNutsball),
    (ActorFunc)EnNutsball_Init,
    (ActorFunc)EnNutsball_Destroy,
    (ActorFunc)EnNutsball_Update,
    (ActorFunc)NULL,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
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
        TOUCH_ON | TOUCH_SFX_WOOD,
        BUMP_ON,
        OCELEM_ON,
    },
    { 13, 13, 0, { 0 } },
};

static s16 sObjectIDs[] = {
    OBJECT_DEKUNUTS, OBJECT_HINTNUTS, OBJECT_SHOPNUTS, OBJECT_DNS, OBJECT_DNK,
};

static Gfx* sDLists[] = {
    gDekuNutsDekuNutDL, gHintNutsNutDL, gBusinessScrubDekuNutDL, gDntJijiNutDL, gDntStageNutDL,
};

void EnNutsball_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnNutsball* thisv = (EnNutsball*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 400.0f, ActorShadow_DrawCircle, 13.0f);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->objBankIndex = Object_GetIndex(&globalCtx->objectCtx, sObjectIDs[thisv->actor.params]);

    if (thisv->objBankIndex < 0) {
        Actor_Kill(&thisv->actor);
    } else {
        thisv->actionFunc = func_80ABBB34;
    }
}

void EnNutsball_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnNutsball* thisv = (EnNutsball*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80ABBB34(EnNutsball* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex)) {
        thisv->actor.objBankIndex = thisv->objBankIndex;
        thisv->actor.draw = EnNutsball_Draw;
        thisv->actor.shape.rot.y = 0;
        thisv->timer = 30;
        thisv->actionFunc = func_80ABBBA8;
        thisv->actor.speedXZ = 10.0f;
    }
}

void func_80ABBBA8(EnNutsball* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3s sp4C;
    Vec3f sp40;

    thisv->timer--;

    if (thisv->timer == 0) {
        thisv->actor.gravity = -1.0f;
    }

    thisv->actor.home.rot.z += 0x2AA8;

    if ((thisv->actor.bgCheckFlags & 8) || (thisv->actor.bgCheckFlags & 1) || (thisv->collider.base.atFlags & AT_HIT) ||
        (thisv->collider.base.acFlags & AC_HIT) || (thisv->collider.base.ocFlags1 & OC1_HIT)) {
        // Checking if the player is using a shield that reflects projectiles
        // And if so, reflects the projectile on impact
        if ((player->currentShield == PLAYER_SHIELD_DEKU) ||
            ((player->currentShield == PLAYER_SHIELD_HYLIAN) && LINK_IS_ADULT)) {
            if ((thisv->collider.base.atFlags & AT_HIT) && (thisv->collider.base.atFlags & AT_TYPE_ENEMY) &&
                (thisv->collider.base.atFlags & AT_BOUNCED)) {
                thisv->collider.base.atFlags &= ~AT_TYPE_ENEMY & ~AT_BOUNCED & ~AT_HIT;
                thisv->collider.base.atFlags |= AT_TYPE_PLAYER;

                thisv->collider.info.toucher.dmgFlags = 2;
                Matrix_MtxFToYXZRotS(&player->shieldMf, &sp4C, 0);
                thisv->actor.world.rot.y = sp4C.y + 0x8000;
                thisv->timer = 30;
                return;
            }
        }

        sp40.x = thisv->actor.world.pos.x;
        sp40.y = thisv->actor.world.pos.y + 4;
        sp40.z = thisv->actor.world.pos.z;

        EffectSsHahen_SpawnBurst(globalCtx, &sp40, 6.0f, 0, 7, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EN_OCTAROCK_ROCK);
        Actor_Kill(&thisv->actor);
    } else {
        if (thisv->timer == -300) {
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnNutsball_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnNutsball* thisv = (EnNutsball*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;

    if (!(player->stateFlags1 & 0x300000C0) || (thisv->actionFunc == func_80ABBB34)) {
        thisv->actionFunc(thisv, globalCtx);

        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10, sCylinderInit.dim.radius, sCylinderInit.dim.height, 5);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

        thisv->actor.flags |= ACTOR_FLAG_24;

        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void EnNutsball_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_nutsball.c", 327);

    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateZ(thisx->home.rot.z * 9.58738e-05f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_nutsball.c", 333),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, sDLists[thisx->params]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_nutsball.c", 337);
}
