/*
 * File: z_bg_haka_tubo.c
 * Overlay: ovl_Bg_Haka_Tubo
 * Description: Shadow Temple Giant Skull Jar
 */

#include "z_bg_haka_tubo.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHakaTubo_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHakaTubo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHakaTubo_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHakaTubo_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgHakaTubo_Idle(BgHakaTubo* thisv, GlobalContext* globalCtx);
void BgHakaTubo_DropCollectible(BgHakaTubo* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Haka_Tubo_InitVars = {
    ACTOR_BG_HAKA_TUBO,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKA_OBJECTS,
    sizeof(BgHakaTubo),
    (ActorFunc)BgHakaTubo_Init,
    (ActorFunc)BgHakaTubo_Destroy,
    (ActorFunc)BgHakaTubo_Update,
    (ActorFunc)BgHakaTubo_Draw,
    NULL,
};

static ColliderCylinderInit sPotColliderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 25, 60, 30, { 0, 0, 0 } },
};

static ColliderCylinderInit sFlamesColliderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 60, 45, 235, { 0, 0, 0 } },
};

static s32 sPotsDestroyed = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaTubo_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaTubo* thisv = (BgHakaTubo*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK3);
    CollisionHeader_GetVirtual(&object_haka_objects_Col_0108B8, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    Collider_InitCylinder(globalCtx, &thisv->potCollider);
    Collider_SetCylinder(globalCtx, &thisv->potCollider, &thisv->dyna.actor, &sPotColliderInit);
    Collider_InitCylinder(globalCtx, &thisv->flamesCollider);
    Collider_SetCylinder(globalCtx, &thisv->flamesCollider, &thisv->dyna.actor, &sFlamesColliderInit);
    thisv->fireScroll = Rand_ZeroOne() * 15.0f;
    sPotsDestroyed = 0;
    thisv->actionFunc = BgHakaTubo_Idle;
}

void BgHakaTubo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaTubo* thisv = (BgHakaTubo*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyCylinder(globalCtx, &thisv->potCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->flamesCollider);
}

void BgHakaTubo_Idle(BgHakaTubo* thisv, GlobalContext* globalCtx) {
    static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    if (thisv->dyna.actor.room == 12) { // 3 spinning pots room in Shadow Temple
        thisv->dyna.actor.shape.rot.y += 0x180;
        thisv->dyna.actor.world.pos.x = Math_SinS(thisv->dyna.actor.shape.rot.y - 0x4000) * 145.0f + -5559.0f;
        thisv->dyna.actor.world.pos.z = Math_CosS(thisv->dyna.actor.shape.rot.y - 0x4000) * 145.0f + -1587.0f;
    }
    // Colliding with flame circle
    if (thisv->flamesCollider.base.atFlags & AT_HIT) {
        thisv->flamesCollider.base.atFlags &= ~AT_HIT;
        func_8002F71C(globalCtx, &thisv->dyna.actor, 5.0f, thisv->dyna.actor.yawTowardsPlayer, 5.0f);
    }
    // Colliding with hitbox inside the pot
    if (thisv->potCollider.base.acFlags & AC_HIT) {
        thisv->potCollider.base.acFlags &= ~AC_HIT;
        // If the colliding actor is within a 50 unit radius and 50 unit height cylinder centered
        // on the actor's position, break the pot
        if (Actor_WorldDistXZToPoint(&thisv->dyna.actor, &thisv->potCollider.base.ac->world.pos) < 50.0f &&
            (thisv->potCollider.base.ac->world.pos.y - thisv->dyna.actor.world.pos.y) < 50.0f) {
            pos.x = thisv->dyna.actor.world.pos.x;
            pos.z = thisv->dyna.actor.world.pos.z;
            pos.y = thisv->dyna.actor.world.pos.y + 80.0f;
            EffectSsBomb2_SpawnLayered(globalCtx, &pos, &sZeroVector, &sZeroVector, 100, 45);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->dyna.actor.world.pos, 50, NA_SE_EV_BOX_BREAK);
            EffectSsHahen_SpawnBurst(globalCtx, &pos, 20.0f, 0, 350, 100, 50, OBJECT_HAKA_OBJECTS, 40,
                                     gEffFragments2DL);
            thisv->dropTimer = 5;
            thisv->dyna.actor.draw = NULL;
            Actor_SetScale(&thisv->dyna.actor, 0.0f);
            thisv->actionFunc = BgHakaTubo_DropCollectible;
        }
    } else {
        Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->flamesCollider);
        Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->potCollider);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->potCollider.base);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->flamesCollider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->flamesCollider.base);
    }
}

void BgHakaTubo_DropCollectible(BgHakaTubo* thisv, GlobalContext* globalCtx) {
    EnItem00* collectible;
    f32 rnd;
    Vec3f spawnPos;
    s32 i;
    s32 collectibleParams;

    thisv->dropTimer--;
    if (thisv->dropTimer == 0) { // Creates a delay between destroying the pot and dropping the collectible
        spawnPos.x = thisv->dyna.actor.world.pos.x;
        spawnPos.y = thisv->dyna.actor.world.pos.y + 200.0f;
        spawnPos.z = thisv->dyna.actor.world.pos.z;
        if (thisv->dyna.actor.room == 12) { // 3 spinning pots room in Shadow Temple
            rnd = Rand_ZeroOne();
            sPotsDestroyed++;
            if (sPotsDestroyed == 3) {
                // All 3 pots destroyed
                collectibleParams = -1;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                // Drop rupees
                for (i = 0; i < 9; i++) {
                    collectible = Item_DropCollectible(globalCtx, &spawnPos, i % 3);
                    if (collectible != NULL) {
                        collectible->actor.velocity.y = 15.0f;
                        collectible->actor.world.rot.y = thisv->dyna.actor.shape.rot.y + (i * 0x1C71);
                    }
                }
            } else if (rnd < 0.2f) {
                // Unlucky, no reward and spawn keese
                collectibleParams = -1;
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_FIREFLY, thisv->dyna.actor.world.pos.x,
                            thisv->dyna.actor.world.pos.y + 80.0f, thisv->dyna.actor.world.pos.z, 0,
                            thisv->dyna.actor.shape.rot.y, 0, 2);
                func_80078884(NA_SE_SY_ERROR);
            } else {
                // Random rewards
                if (rnd < 0.4f) {
                    collectibleParams = ITEM00_BOMBS_A;
                } else if (rnd < 0.6f) {
                    collectibleParams = ITEM00_MAGIC_LARGE;
                } else if (rnd < 0.8f) {
                    collectibleParams = ITEM00_MAGIC_SMALL;
                } else {
                    collectibleParams = ITEM00_ARROWS_SMALL;
                }
                func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            }
        } else if (Flags_GetCollectible(globalCtx, thisv->dyna.actor.params) != 0) {
            // If small key already collected, drop recovery heart instead
            collectibleParams = ITEM00_HEART;
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            // Drops a small key and sets a collect flag
            collectibleParams = ((thisv->dyna.actor.params & 0x3F) << 8) | ITEM00_SMALL_KEY;
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }
        if (collectibleParams != -1) {
            collectible = Item_DropCollectible(globalCtx, &spawnPos, collectibleParams);
            if (collectible != NULL) {
                collectible->actor.velocity.y = 15.0f;
                collectible->actor.world.rot.y = thisv->dyna.actor.shape.rot.y;
            }
        }
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgHakaTubo_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaTubo* thisv = (BgHakaTubo*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    thisv->fireScroll++;
}

void BgHakaTubo_DrawFlameCircle(BgHakaTubo* thisv, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_haka_tubo.c", 476);

    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_Translate(thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y + 235.0f, thisv->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateY(thisv->dyna.actor.shape.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(0.07f, 0.04f, 0.07f, MTXMODE_APPLY);
    if (1) {}
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 170, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, thisv->fireScroll & 127, 0, 32, 64, 1, 0,
                                (thisv->fireScroll * -15) & 0xFF, 32, 64));
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_haka_tubo.c", 497),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_haka_tubo.c", 501);
}

void BgHakaTubo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaTubo* thisv = (BgHakaTubo*)thisx;

    Gfx_DrawDListOpa(globalCtx, object_haka_objects_DL_00FE40);
    BgHakaTubo_DrawFlameCircle(thisv, globalCtx);
}
