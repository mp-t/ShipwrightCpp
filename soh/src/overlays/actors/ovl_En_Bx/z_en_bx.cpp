/*
 * File: z_en_bx.c
 * Overlay: ovl_En_Bx
 * Description: Jabu-Jabu Electrified Tentacle
 */

#include "z_en_bx.h"
#include "objects/object_bxa/object_bxa.h"

#define FLAGS ACTOR_FLAG_4

void EnBx_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBx_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBx_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBx_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit En_Bx_InitVars = {
    ACTOR_EN_BX,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BXA,
    sizeof(EnBx),
    (ActorFunc)EnBx_Init,
    (ActorFunc)EnBx_Destroy,
    (ActorFunc)EnBx_Update,
    (ActorFunc)EnBx_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0xFFCFFFFF, 0x03, 0x04 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 60, 100, 100, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit = {
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
        { 0xFFCFFFFF, 0x03, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

void EnBx_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBx* thisv = (EnBx*)thisx;
    Vec3f sp48 = { 0.015f, 0.015f, 0.015f };
    Vec3f sp3C = { 0.0f, 0.0f, 0.0f };
    static InitChainEntry sInitChain[] = {
        ICHAIN_F32(targetArrowOffset, 5300, ICHAIN_STOP),
    };
    s32 i;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisx->scale.x = thisx->scale.z = 0.01f;
    thisx->scale.y = 0.03f;

    thisx->world.pos.y = thisx->world.pos.y - 100.0f;
    for (i = 0; i < 4; i++) {
        thisv->unk_184[i] = sp48;
        if (i == 0) {
            thisv->unk_1B4[i].x = thisx->shape.rot.x - 0x4000;
        }
        thisv->unk_154[i] = thisx->world.pos;
        thisv->unk_154[i].y = thisx->world.pos.y + ((i + 1) * 140.0f);
    }

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    Collider_InitQuad(globalCtx, &thisv->colliderQuad);
    Collider_SetQuad(globalCtx, &thisv->colliderQuad, &thisv->actor, &sQuadInit);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    thisv->unk_14C = 0;
    thisx->uncullZoneDownward = 2000.0f;
    if (Flags_GetSwitch(globalCtx, (thisx->params >> 8) & 0xFF)) {
        Actor_Kill(&thisv->actor);
    }
    thisx->params &= 0xFF;
}

void EnBx_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBx* thisv = (EnBx*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_809D1D0C(Actor* thisx, GlobalContext* globalCtx) {
    Vec3f sp5C = { 8000.0f, 15000.0f, 2500.0f };
    Vec3f sp50 = { 8000.0f, 10000.0f, 2500.0f };
    static Vec3f D_809D2540 = { -8000.0f, 15000.0f, 2500.0f };
    static Vec3f D_809D254C = { -8000.0f, 10000.0f, 2500.0f };
    Vec3f sp44;
    Vec3f sp38;
    EnBx* thisv = (EnBx*)thisx;

    Matrix_MultVec3f(&D_809D2540, &sp44);
    Matrix_MultVec3f(&D_809D254C, &sp38);
    Matrix_MultVec3f(&sp5C, &thisv->colliderQuad.dim.quad[1]);
    Matrix_MultVec3f(&sp50, &thisv->colliderQuad.dim.quad[0]);
    Collider_SetQuadVertices(&thisv->colliderQuad, &sp38, &sp44, &thisv->colliderQuad.dim.quad[0],
                             &thisv->colliderQuad.dim.quad[1]);
}

void EnBx_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBx* thisv = (EnBx*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    s32 i;
    s16 tmp32;
    s32 tmp33;

    if ((thisx->xzDistToPlayer <= 70.0f) || (thisv->collider.base.atFlags & AT_HIT) ||
        (thisv->collider.base.acFlags & AC_HIT) || (thisv->colliderQuad.base.atFlags & AT_HIT)) {
        if ((thisx->xzDistToPlayer <= 70.0f) || (&player->actor == thisv->collider.base.at) ||
            (&player->actor == thisv->collider.base.ac) || (&player->actor == thisv->colliderQuad.base.at)) {
            tmp33 = player->invincibilityTimer & 0xFF;
            tmp32 = thisx->world.rot.y;
            if (!(thisx->params & 0x80)) {
                tmp32 = thisx->yawTowardsPlayer;
            }
            if ((&player->actor != thisv->collider.base.at) && (&player->actor != thisv->collider.base.ac) &&
                (&player->actor != thisv->colliderQuad.base.at) && (player->invincibilityTimer <= 0)) {
                if (player->invincibilityTimer < -39) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    globalCtx->damagePlayer(globalCtx, -4);
                }
            }
            func_8002F71C(globalCtx, &thisv->actor, 6.0f, tmp32, 6.0f);
            player->invincibilityTimer = tmp33;
        }

        thisv->collider.base.atFlags &= ~AT_HIT;
        thisv->collider.base.acFlags &= ~AC_HIT;
        thisv->colliderQuad.base.atFlags &= ~AT_HIT;
        thisv->colliderQuad.base.at = NULL;
        thisv->collider.base.ac = NULL;
        thisv->collider.base.at = NULL;
        thisv->unk_14C = 0x14;
    }

    if (thisv->unk_14C != 0) {
        thisv->unk_14C--;
        for (i = 0; i < 4; i++) {
            if (!((thisv->unk_14C + (i << 1)) % 4)) {
                static Color_RGBA8 primColor = { 255, 255, 255, 255 };
                static Color_RGBA8 envColor = { 200, 255, 255, 255 };
                Vec3f pos;
                s16 yaw;

                yaw = (s32)Rand_CenteredFloat(12288.0f);
                yaw = (yaw + (i * 0x4000)) + 0x2000;
                pos.x = Rand_CenteredFloat(5.0f) + thisx->world.pos.x;
                pos.y = Rand_CenteredFloat(30.0f) + thisx->world.pos.y + 170.0f;
                pos.z = Rand_CenteredFloat(5.0f) + thisx->world.pos.z;
                EffectSsLightning_Spawn(globalCtx, &pos, &primColor, &envColor, 230, yaw, 6, 0);
            }
        }

        Audio_PlayActorSound2(thisx, NA_SE_EN_BIRI_SPARK - SFX_FLAG);
    }
    thisx->focus.pos = thisx->world.pos;
    Collider_UpdateCylinder(thisx, &thisv->collider);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    if (thisx->params & 0x80) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderQuad.base);
    }
}

void EnBx_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* D_809D2560[] = {
        object_bxa_Tex_0024F0,
        object_bxa_Tex_0027F0,
        object_bxa_Tex_0029F0,
    };
    EnBx* thisv = (EnBx*)thisx;
    s32 pad;
    Mtx* mtx = Graph_Alloc(globalCtx->state.gfxCtx, 4 * sizeof(Mtx));
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bx.c", 464);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, mtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_809D2560[thisv->actor.params & 0x7F]));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 16, 16, 1, 0, (globalCtx->gameplayFrames * -10) % 128,
                                32, 32));
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bx.c", 478),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (thisv->actor.params & 0x80) {
        func_809D1D0C(&thisv->actor, globalCtx);
    }

    thisv->unk_14E -= 0xBB8;
    thisx->scale.z = thisx->scale.x = (Math_CosS(thisv->unk_14E) * 0.0075f) + 0.015f;

    for (i = 3; i >= 0; i--) {
        s16 off = (0x2000 * i);

        thisv->unk_184[i].z = thisv->unk_184[i].x = (Math_CosS(thisv->unk_14E + off) * 0.0075f) + 0.015f;
        thisv->unk_1B4[i].x = thisx->shape.rot.x;
        thisv->unk_1B4[i].y = thisx->shape.rot.y;
        thisv->unk_1B4[i].z = thisx->shape.rot.z;
    }

    for (i = 0; i < 4; i++, mtx++) {
        Matrix_Translate(thisv->unk_154[i].x, thisv->unk_154[i].y, thisv->unk_154[i].z, MTXMODE_NEW);
        Matrix_RotateZYX(thisv->unk_1B4[i].x, thisv->unk_1B4[i].y, thisv->unk_1B4[i].z, MTXMODE_APPLY);
        Matrix_Scale(thisv->unk_184[i].x, thisv->unk_184[i].y, thisv->unk_184[i].z, MTXMODE_APPLY);
        Matrix_ToMtx(mtx, "../z_en_bx.c", 507);
    }

    gSPDisplayList(POLY_OPA_DISP++, object_bxa_DL_0022F0);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bx.c", 511);
}
