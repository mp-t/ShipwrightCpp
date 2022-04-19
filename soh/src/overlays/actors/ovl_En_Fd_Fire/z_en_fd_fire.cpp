#include "z_en_fd_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnFdFire_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFdFire_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFdFire_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFdFire_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnFdFire_Disappear(EnFdFire* thisv, GlobalContext* globalCtx);
void func_80A0E70C(EnFdFire* thisv, GlobalContext* globalCtx);
void EnFdFire_DanceTowardsPlayer(EnFdFire* thisv, GlobalContext* globalCtx);
void EnFdFire_WaitToDie(EnFdFire* thisv, GlobalContext* globalCtx);

ActorInit En_Fd_Fire_InitVars = {
    ACTOR_EN_FD_FIRE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(EnFdFire),
    (ActorFunc)EnFdFire_Init,
    (ActorFunc)EnFdFire_Destroy,
    (ActorFunc)EnFdFire_Update,
    (ActorFunc)EnFdFire_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x08 },
        { 0x0D840008, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(4, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

void EnFdFire_UpdatePos(EnFdFire* thisv, Vec3f* targetPos) {
    f32 dist;
    f32 xDiff = targetPos->x - thisv->actor.world.pos.x;
    f32 yDiff = targetPos->y - thisv->actor.world.pos.y;
    f32 zDiff = targetPos->z - thisv->actor.world.pos.z;

    dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    if (fabsf(dist) > fabsf(thisv->actor.speedXZ)) {
        thisv->actor.velocity.x = (xDiff / dist) * thisv->actor.speedXZ;
        thisv->actor.velocity.z = (zDiff / dist) * thisv->actor.speedXZ;
    } else {
        thisv->actor.velocity.x = 0.0f;
        thisv->actor.velocity.z = 0.0f;
    }

    thisv->actor.velocity.y += thisv->actor.gravity;
    if (!(thisv->actor.minVelocityY <= thisv->actor.velocity.y)) {
        thisv->actor.velocity.y = thisv->actor.minVelocityY;
    }
}

s32 EnFdFire_CheckCollider(EnFdFire* thisv, GlobalContext* globalCtx) {
    if (thisv->actionFunc == EnFdFire_Disappear) {
        return false;
    }

    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        return true;
    }

    if (thisv->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        thisv->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        return true;
    }
    return false;
}

void EnFdFire_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFdFire* thisv = (EnFdFire*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInit);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.gravity = -0.6f;
    thisv->actor.speedXZ = 5.0f;
    thisv->actor.velocity.y = 12.0f;
    thisv->spawnRadius = Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &player->actor.world.pos);
    thisv->scale = 3.0f;
    thisv->tile2Y = (s16)Rand_ZeroFloat(5.0f) - 25;
    thisv->actionFunc = func_80A0E70C;
}

void EnFdFire_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFdFire* thisv = (EnFdFire*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80A0E70C(EnFdFire* thisv, GlobalContext* globalCtx) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f targetPos = thisv->actor.parent->world.pos;

    targetPos.x += thisv->spawnRadius * Math_SinS(thisv->actor.world.rot.y);
    targetPos.z += thisv->spawnRadius * Math_CosS(thisv->actor.world.rot.y);
    EnFdFire_UpdatePos(thisv, &targetPos);
    if (thisv->actor.bgCheckFlags & 1 && (!(thisv->actor.velocity.y > 0.0f))) {
        thisv->actor.velocity = velocity;
        thisv->actor.speedXZ = 0.0f;
        thisv->actor.bgCheckFlags &= ~1;
        if (thisv->actor.params & 0x8000) {
            thisv->deathTimer = 200;
            thisv->actionFunc = EnFdFire_DanceTowardsPlayer;
        } else {
            thisv->deathTimer = 300;
            thisv->actionFunc = EnFdFire_WaitToDie;
        }
    }
}

void EnFdFire_WaitToDie(EnFdFire* thisv, GlobalContext* globalCtx) {
    if (DECR(thisv->deathTimer) == 0) {
        thisv->actionFunc = EnFdFire_Disappear;
    }
}

void EnFdFire_DanceTowardsPlayer(EnFdFire* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 angles[] = {
        0.0f, 210.0f, 60.0f, 270.0f, 120.0f, 330.0f, 180.0f, 30.0f, 240.0f, 90.0f, 300.0f, 150.0f,
    };
    Vec3f pos;
    s16 idx;

    idx = ((globalCtx->state.frames / 10) + (thisv->actor.params & 0x7FFF)) % ARRAY_COUNT(angles);
    pos = player->actor.world.pos;
    pos.x += 120.0f * sinf(angles[idx]);
    pos.z += 120.0f * cosf(angles[idx]);

    if (DECR(thisv->deathTimer) == 0) {
        thisv->actionFunc = EnFdFire_Disappear;
    } else {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_Vec3f_Yaw(&thisv->actor.world.pos, &pos), 8, 0xFA0, 1);
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.4f, 1.0f, 0.0f);
        if (thisv->actor.speedXZ < 0.1f) {
            thisv->actor.speedXZ = 5.0f;
        }
        func_8002D868(&thisv->actor);
    }
}

void EnFdFire_Disappear(EnFdFire* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.6f, 9.0f, 0.0f);
    func_8002D868(&thisv->actor);
    Math_SmoothStepToF(&thisv->scale, 0.0f, 0.3f, 0.1f, 0.0f);
    thisv->actor.shape.shadowScale = 20.0f;
    thisv->actor.shape.shadowScale *= (thisv->scale / 3.0f);
    if (!(thisv->scale > 0.01f)) {
        Actor_Kill(&thisv->actor);
    }
}

void EnFdFire_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFdFire* thisv = (EnFdFire*)thisx;
    s32 pad;

    if (thisv->actionFunc != EnFdFire_Disappear) {
        if ((thisv->actor.parent->update == NULL) || EnFdFire_CheckCollider(thisv, globalCtx)) {
            thisv->actionFunc = EnFdFire_Disappear;
        }
    }

    func_8002D7EC(&thisv->actor);
    thisv->actionFunc(thisv, globalCtx);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 12.0f, 10.0f, 0.0f, 5);

    if (thisv->actionFunc != EnFdFire_Disappear) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void EnFdFire_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Color_RGBA8 primColors[] = {
        { 255, 255, 0, 255 },
        { 255, 255, 255, 255 },
    };
    Color_RGBA8 envColors[] = {
        { 255, 10, 0, 255 },
        { 0, 10, 255, 255 },
    };
    s32 pad;
    EnFdFire* thisv = (EnFdFire*)thisx;
    Vec3f scale = { 0.0f, 0.0f, 0.0f };
    Vec3f sp90 = { 0.0f, 0.0f, 0.0f };
    s16 sp8E;
    f32 sp88;
    f32 sp84;
    f32 sp80;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fd_fire.c", 572);

    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
    sp8E = Math_Vec3f_Yaw(&scale, &thisv->actor.velocity) - Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx));
    sp84 = fabsf(Math_CosS(sp8E));
    sp88 = Math_SinS(sp8E);
    sp80 = Math_Vec3f_DistXZ(&scale, &thisv->actor.velocity) / 1.5f;
    if (1) {}
    if (1) {}
    if (1) {}
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000) * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateZ(((sp88 * -10.0f) * sp80) * (std::numbers::pi_v<float> / 180.0f), MTXMODE_APPLY);
    scale.x = scale.y = scale.z = thisv->scale * 0.001f;
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    sp84 = sp80 * ((0.01f * -15.0f) * sp84) + 1.0f;
    if (sp84 < 0.1f) {
        sp84 = 0.1f;
    }
    Matrix_Scale(1.0f, sp84, 1.0f / sp84, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fd_fire.c", 623),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                globalCtx->state.frames * thisv->tile2Y, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, primColors[((thisv->actor.params & 0x8000) >> 0xF)].r,
                    primColors[((thisv->actor.params & 0x8000) >> 0xF)].g,
                    primColors[((thisv->actor.params & 0x8000) >> 0xF)].b,
                    primColors[((thisv->actor.params & 0x8000) >> 0xF)].a);
    gDPSetEnvColor(POLY_XLU_DISP++, envColors[((thisv->actor.params & 0x8000) >> 0xF)].r,
                   envColors[((thisv->actor.params & 0x8000) >> 0xF)].g,
                   envColors[((thisv->actor.params & 0x8000) >> 0xF)].b,
                   envColors[((thisv->actor.params & 0x8000) >> 0xF)].a);
    gDPPipeSync(POLY_XLU_DISP++);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fd_fire.c", 672);
}
