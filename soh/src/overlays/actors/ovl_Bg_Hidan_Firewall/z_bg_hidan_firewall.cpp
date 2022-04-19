/*
 * File: z_bg_hidan_firewall.c
 * Overlay: ovl_Bg_Hidan_Firewall
 * Description: Proximity Triggered Flame Wall
 */

#include "z_bg_hidan_firewall.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanFirewall_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFirewall_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFirewall_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFirewall_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 BgHidanFirewall_CheckProximity(BgHidanFirewall* thisv, GlobalContext* globalCtx);
void BgHidanFirewall_Wait(BgHidanFirewall* thisv, GlobalContext* globalCtx);
void BgHidanFirewall_Countdown(BgHidanFirewall* thisv, GlobalContext* globalCtx);
void BgHidanFirewall_Erupt(BgHidanFirewall* thisv, GlobalContext* globalCtx);
void BgHidanFirewall_Collide(BgHidanFirewall* thisv, GlobalContext* globalCtx);
void BgHidanFirewall_ColliderFollowPlayer(BgHidanFirewall* thisv, GlobalContext* globalCtx);

ActorInit Bg_Hidan_Firewall_InitVars = {
    ACTOR_BG_HIDAN_FIREWALL,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanFirewall),
    (ActorFunc)BgHidanFirewall_Init,
    (ActorFunc)BgHidanFirewall_Destroy,
    (ActorFunc)BgHidanFirewall_Update,
    NULL,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
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
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 30, 83, 0, { 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 80, 100, MASS_IMMOVABLE };

void BgHidanFirewall_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanFirewall* thisv = (BgHidanFirewall*)thisx;

    thisv->actor.scale.x = 0.12f;
    thisv->actor.scale.z = 0.12f;
    thisv->actor.scale.y = 0.01f;

    thisv->unk_150 = 0;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);

    thisv->collider.dim.pos.y = thisv->actor.world.pos.y;

    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);

    thisv->actionFunc = BgHidanFirewall_Wait;
}

void BgHidanFirewall_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanFirewall* thisv = (BgHidanFirewall*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 BgHidanFirewall_CheckProximity(BgHidanFirewall* thisv, GlobalContext* globalCtx) {
    Player* player;
    Vec3f distance;

    player = GET_PLAYER(globalCtx);
    func_8002DBD0(&thisv->actor, &distance, &player->actor.world.pos);

    if (fabsf(distance.x) < 100.0f && fabsf(distance.z) < 120.0f) {
        return 1;
    }
    return 0;
}

void BgHidanFirewall_Wait(BgHidanFirewall* thisv, GlobalContext* globalCtx) {
    if (BgHidanFirewall_CheckProximity(thisv, globalCtx) != 0) {
        thisv->actor.draw = BgHidanFirewall_Draw;
        thisv->actor.params = 5;
        thisv->actionFunc = BgHidanFirewall_Countdown;
    }
}

void BgHidanFirewall_Countdown(BgHidanFirewall* thisv, GlobalContext* globalCtx) {

    if (thisv->actor.params != 0) {
        thisv->actor.params--;
    }
    if (thisv->actor.params == 0) {
        thisv->actionFunc = BgHidanFirewall_Erupt;
    }
}

void BgHidanFirewall_Erupt(BgHidanFirewall* thisv, GlobalContext* globalCtx) {
    if (BgHidanFirewall_CheckProximity(thisv, globalCtx) != 0) {
        Math_StepToF(&thisv->actor.scale.y, 0.1f, 0.01f / 0.4f);
    } else {
        if (Math_StepToF(&thisv->actor.scale.y, 0.01f, 0.01f) != 0) {
            thisv->actor.draw = NULL;
            thisv->actionFunc = BgHidanFirewall_Wait;
        } else {
            thisv->actor.params = 0;
        }
    }
}

void BgHidanFirewall_Collide(BgHidanFirewall* thisv, GlobalContext* globalCtx) {
    s16 phi_a3;

    if (Actor_IsFacingPlayer(&thisv->actor, 0x4000)) {
        phi_a3 = thisv->actor.shape.rot.y;
    } else {
        phi_a3 = thisv->actor.shape.rot.y + 0x8000;
    }

    func_8002F71C(globalCtx, &thisv->actor, 5.0f, phi_a3, 1.0f);
}

void BgHidanFirewall_ColliderFollowPlayer(BgHidanFirewall* thisv, GlobalContext* globalCtx) {
    Player* player;
    Vec3f sp30;
    f32 temp_ret;
    f32 sp28;
    f32 phi_f0;

    player = GET_PLAYER(globalCtx);

    func_8002DBD0(&thisv->actor, &sp30, &player->actor.world.pos);
    if (sp30.x < -70.0f) {
        sp30.x = -70.0f;
    } else {
        if (70.0f < sp30.x) {
            phi_f0 = 70.0f;
        } else {
            phi_f0 = sp30.x;
        }
        sp30.x = phi_f0;
    }
    if (thisv->actor.params == 0) {
        if (0.0f < sp30.z) {
            sp30.z = -25.0f;
            thisv->actor.params = -1;
        } else {
            sp30.z = 25.0f;
            thisv->actor.params = 1;
        }
    } else {
        sp30.z = thisv->actor.params * 25.0f;
    }
    sp28 = Math_SinS(thisv->actor.shape.rot.y);
    temp_ret = Math_CosS(thisv->actor.shape.rot.y);
    thisv->collider.dim.pos.x = thisv->actor.world.pos.x + sp30.x * temp_ret + sp30.z * sp28;
    thisv->collider.dim.pos.z = thisv->actor.world.pos.z - sp30.x * sp28 + sp30.z * temp_ret;
}

void BgHidanFirewall_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanFirewall* thisv = (BgHidanFirewall*)thisx;
    s32 pad;

    thisv->unk_150 = (thisv->unk_150 + 1) % 8;

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        BgHidanFirewall_Collide(thisv, globalCtx);
    }

    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actionFunc == BgHidanFirewall_Erupt) {
        BgHidanFirewall_ColliderFollowPlayer(thisv, globalCtx);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        func_8002F974(&thisv->actor, NA_SE_EV_FIRE_PLATE - SFX_FLAG);
    }
}

static const void* sFireballTexs[] = {
    gFireTempleFireball0Tex, gFireTempleFireball1Tex, gFireTempleFireball2Tex, gFireTempleFireball3Tex,
    gFireTempleFireball4Tex, gFireTempleFireball5Tex, gFireTempleFireball6Tex, gFireTempleFireball7Tex,
};

void BgHidanFirewall_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanFirewall* thisv = (BgHidanFirewall*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_firewall.c", 448);

    POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sFireballTexs[thisv->unk_150]));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x01, 255, 255, 0, 150);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_hidan_firewall.c", 458),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gFireTempleFireballUpperHalfDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_firewall.c", 463);
}
