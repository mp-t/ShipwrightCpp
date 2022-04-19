#include "z_bg_spot16_bombstone.h"
#include "objects/object_spot16_obj/object_spot16_obj.h"
#include "objects/object_bombiwa/object_bombiwa.h"
#include "overlays/actors/ovl_En_Bombf/z_en_bombf.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS ACTOR_FLAG_4

void BgSpot16Bombstone_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Bombstone_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Bombstone_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Bombstone_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Bombstone_Reset(void);

void func_808B5A94(BgSpot16Bombstone* thisv, GlobalContext* globalCtx);
void func_808B5B04(BgSpot16Bombstone* thisv, GlobalContext* globalCtx);
void func_808B5B6C(BgSpot16Bombstone* thisv, GlobalContext* globalCtx);
void func_808B5B58(BgSpot16Bombstone* thisv);
void func_808B5950(BgSpot16Bombstone* thisv, GlobalContext* globalCtx);
void func_808B5934(BgSpot16Bombstone* thisv);
void func_808B5AF0(BgSpot16Bombstone* thisv);
void func_808B5A78(BgSpot16Bombstone* thisv);

static EnBombf* sPlayerBomb = NULL;

static s16 sTimer = 0;

static s16 D_808B5DD8[][10] = {
    { 0x0008, 0x0004, 0x0046, 0x07D0, 0xFCE0, 0x0000, 0x0064, 0x0000, 0x0000, 0x0000 },
    { 0x0006, 0x0003, 0x0032, 0x00C8, 0x0A28, 0xC350, 0x005A, 0x0000, 0x0000, 0x0000 },
    { 0x0005, 0x0003, 0x0028, 0xF63C, 0x0190, 0x30B0, 0x0032, 0x0000, 0x0000, 0x0000 },
    { 0x0003, 0x0001, 0x003C, 0x0258, 0xFF9C, 0xAFC8, 0x0032, 0x0000, 0x0000, 0x0000 },
    { 0x0003, 0x0001, 0x0028, 0xF2B8, 0xFF9C, 0x6590, 0x001E, 0x0000, 0x0000, 0x0000 },
    { 0x0006, 0x0009, 0x0028, 0x0000, 0x0BB8, 0xD8F0, 0x001E, 0x0000, 0x0000, 0x0000 },
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4FC1FFF6, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 50, 0 }, 288 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
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
    { 190, 80, 0, { 10, 0, 50 } },
};

static s16 D_808B5EB0[][7] = {
    { 0x0000, 0x000A, 0x003C, 0xFFF6, 0x0104, 0x01E0, 0x0007 },
    { 0x0000, 0x0000, 0x0032, 0x0000, 0x0104, 0x017C, 0x000D },
    { 0x0001, 0x001E, 0x0014, 0x0014, 0x00A0, 0x0104, 0x000A },
    { 0x0002, 0x0014, 0x0028, 0x0014, 0x00C8, 0x0096, 0x0007 },
    { 0x0003, 0xFFD8, 0x0064, 0x001E, 0x00D2, 0x0064, 0x0007 },
    { 0x0004, 0x000A, 0x0078, 0x000A, 0x00A0, 0x0028, 0x000A },
    { 0x0005, 0xFFA7, 0x006E, 0x0041, 0x0096, 0x0118, 0x0007 },
    { 0x0006, 0xFFC4, 0x0070, 0x006D, 0x006E, 0x0082, 0x0007 },
    { 0x0007, 0xFFF2, 0x0050, 0x007F, 0x008C, 0x00D2, 0x0007 },
    { 0x0008, 0x0014, 0x0032, 0x001E, 0x008C, 0x00C8, 0x0007 },
    { 0x0009, 0xFFEC, 0x0014, 0x0028, 0x00C8, 0x00B4, 0x0007 },
    { 0x000A, 0x001E, 0x0028, 0xFFE2, 0x0064, 0x0078, 0x0007 },
    { 0x000A, 0xFF92, 0x001D, 0x0078, 0x008C, 0x0118, 0x000A },
    { 0x000B, 0x001E, 0x0014, 0x001E, 0x0050, 0x00C8, 0x0006 },
    { 0x000C, 0x0028, 0x001E, 0xFFE2, 0x00AA, 0x010E, 0x0011 },
    { 0x000D, 0xFFD8, 0x0032, 0xFFEC, 0x008C, 0x0056, 0x0006 },
    { 0x000D, 0x0032, 0x0032, 0x0032, 0x0096, 0x00C8, 0x000A },
    { 0x000E, 0x0028, 0x0028, 0xFFD8, 0x003C, 0x00A0, 0x0006 },
    { 0x000F, 0xFFE2, 0x0014, 0x0032, 0x00AA, 0x012C, 0x0006 },
    { 0x0010, 0x0028, 0x0032, 0x0014, 0x00AA, 0x0078, 0x0007 },
    { 0x0010, 0x001E, 0x0032, 0x003C, 0x0096, 0x00C8, 0x0006 },
    { 0x0011, 0x000A, 0x000A, 0x0000, 0x0082, 0x00DC, 0x0006 },
    { 0x0012, 0x000A, 0x0028, 0x0014, 0x00B4, 0x00DC, 0x000B },
    { 0x0013, 0x000A, 0x0005, 0x0000, 0x006E, 0x0046, 0x0011 },
    { 0x0013, 0xFFEC, 0x0032, 0xFFE2, 0x0096, 0x00C8, 0x0006 },
    { 0x0014, 0x0050, 0x0032, 0x0000, 0x0096, 0x00C8, 0x0008 },
};

ActorInit Bg_Spot16_Bombstone_InitVars = {
    ACTOR_BG_SPOT16_BOMBSTONE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SPOT16_OBJ,
    sizeof(BgSpot16Bombstone),
    (ActorFunc)BgSpot16Bombstone_Init,
    (ActorFunc)BgSpot16Bombstone_Destroy,
    (ActorFunc)BgSpot16Bombstone_Update,
    (ActorFunc)BgSpot16Bombstone_Draw,
    (ActorResetFunc)BgSpot16Bombstone_Reset
};

static InitChainEntry sInitChainBoulder[] = {
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static InitChainEntry sInitChainDebris[] = {
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static Vec3f sVelocity = { 0.0f, 0.0f, 0.0f };

static Vec3f sAcceleration = { 0.0f, 0.4f, 0.0f };

static f32 D_808B6074[] = { 66.0f, 51.0f, 48.0f, 36.0f, 21.0f };

static s16 D_808B6088[] = { 0, 1, 2, 3, 4 };

void func_808B4C30(BgSpot16Bombstone* thisv) {
    thisv->switchFlag = (thisv->actor.params >> 8) & 0x3F;
    thisv->actor.params = thisv->actor.params & 0xFF;
}

void func_808B4C4C(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->colliderJntSph);
    Collider_SetJntSph(globalCtx, &thisv->colliderJntSph, &thisv->actor, &sJntSphInit, thisv->colliderElements);
    thisv->colliderJntSph.elements[0].dim.worldSphere.center.x = thisv->actor.world.pos.x;
    thisv->colliderJntSph.elements[0].dim.worldSphere.center.y = thisv->actor.world.pos.y + 50.0f;
    thisv->colliderJntSph.elements[0].dim.worldSphere.center.z = thisv->actor.world.pos.z;
    thisv->colliderJntSph.elements[0].dim.worldSphere.radius = 120;
}

void func_808B4D04(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitCylinder(globalCtx, &thisv->colliderCylinder);
    Collider_SetCylinder(globalCtx, &thisv->colliderCylinder, &thisv->actor, &sCylinderInit);
    thisv->colliderCylinder.dim.pos.x += (s16)thisv->actor.world.pos.x;
    thisv->colliderCylinder.dim.pos.y += (s16)thisv->actor.world.pos.y;
    thisv->colliderCylinder.dim.pos.z += (s16)thisv->actor.world.pos.z;
}

s32 func_808B4D9C(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        osSyncPrintf("Spot16 obj 爆弾石 破壊済み\n");
        return false;
    }
    Actor_ProcessInitChain(&thisv->actor, sInitChainBoulder);
    Actor_SetScale(&thisv->actor, 0.4f);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    func_808B4C4C(thisv, globalCtx);
    func_808B4D04(thisv, globalCtx);
    thisv->sinRotation = Math_SinS(thisv->actor.shape.rot.y);
    thisv->cosRotation = Math_CosS(thisv->actor.shape.rot.y);
    thisv->dList = gDodongosCavernRock3DL;

    func_808B5934(thisv);
    return true;
}

s32 func_808B4E58(BgSpot16Bombstone* thisv, GlobalContext* globalctx) {
    Actor* actor = &thisv->actor;
    f32 scaleFactor = 1.0f / 600.0f;
    f32 sinCosPosFactor = 50.0f;
    f32 sinValue;
    f32 cosValue;

    Actor_ProcessInitChain(actor, sInitChainDebris);

    actor->speedXZ = D_808B5DD8[actor->params][0];
    actor->velocity.y = D_808B5DD8[actor->params][1];

    Actor_SetScale(actor, D_808B5DD8[actor->params][2] * scaleFactor);

    thisv->unk_210 = (f32)D_808B5DD8[actor->params][3];
    thisv->unk_212 = (f32)D_808B5DD8[actor->params][4];

    actor->world.rot.y = D_808B5DD8[actor->params][5];

    sinValue = Math_SinS(thisv->actor.world.rot.y);
    cosValue = Math_CosS(thisv->actor.world.rot.y);

    actor->world.pos.x = (sinValue * sinCosPosFactor) + actor->home.pos.x;
    actor->world.pos.y = D_808B5DD8[actor->params][6] + actor->home.pos.y;
    actor->world.pos.z = (cosValue * sinCosPosFactor) + actor->home.pos.z;

    actor->shape.rot.x = D_808B5DD8[actor->params][7];
    actor->shape.rot.y = D_808B5DD8[actor->params][8];
    actor->shape.rot.z = D_808B5DD8[actor->params][9];

    thisv->dList = object_bombiwa_DL_0009E0;
    thisv->bombiwaBankIndex = Object_GetIndex(&globalctx->objectCtx, OBJECT_BOMBIWA);

    if (thisv->bombiwaBankIndex < 0) {
        osSyncPrintf("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", actor->params, "../z_bg_spot16_bombstone.c",
                     589);
        return false;
    }

    func_808B5AF0(thisv);
    return true;
}

void BgSpot16Bombstone_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Bombstone* thisv = (BgSpot16Bombstone*)thisx;
    s16 shouldLive;

    func_808B4C30(thisv);

    switch (thisv->actor.params) {
        case 0xFF:
            // The boulder is intact
            shouldLive = func_808B4D9C(thisv, globalCtx);
            break;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            // The boulder is debris
            shouldLive = func_808B4E58(thisv, globalCtx);
            break;
        default:
            osSyncPrintf("Error : arg_data おかしいな(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot16_bombstone.c", 668,
                         thisv->actor.params);
            shouldLive = false;
            break;
    }

    if (!shouldLive) {
        Actor_Kill(&thisv->actor);
        return;
    }
    osSyncPrintf("Spot16 obj 爆弾石 (scaleX %f)(arg_data 0x%04x)\n", thisv->actor.scale.x, thisv->actor.params);
}

void BgSpot16Bombstone_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Bombstone* thisv = (BgSpot16Bombstone*)thisx;

    if (thisv->actor.params == 0xFF) {
        // Boulder is intact so remove its collider
        Collider_DestroyJntSph(globalCtx, &thisv->colliderJntSph);
        Collider_DestroyCylinder(globalCtx, &thisv->colliderCylinder);
    }
}

void BgSpot16Bombstone_SpawnDust(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    f32 scaleX1 = thisv->actor.scale.x * 150;
    s16 scaleX2 = thisv->actor.scale.x * 250;
    Vec3f world;

    world.x = thisv->actor.world.pos.x;
    world.y = thisv->actor.world.pos.y + 50.0f;
    world.z = thisv->actor.world.pos.z;

    func_80033480(globalCtx, &world, scaleX1, 2, scaleX2, 0xA0, 1);
}

void func_808B5240(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    f32 tempUnk6;
    f32 tempUnk2;
    s16 index;
    Vec3f position;
    Vec3f* actorPosition = &thisv->actor.world.pos;

    if (1) {}

    while (true) {
        if ((u32)thisv->unk_158 >= ARRAY_COUNTU(D_808B5EB0) || thisv->unk_154 < D_808B5EB0[thisv->unk_158][0]) {
            break;
        }

        index = thisv->unk_158;

        tempUnk2 = D_808B5EB0[index][1];
        tempUnk6 = D_808B5EB0[index][3];

        position.x = ((thisv->sinRotation * tempUnk6) + (tempUnk2 * thisv->cosRotation)) + actorPosition->x;
        position.y = D_808B5EB0[index][2] + actorPosition->y;
        position.z = ((thisv->cosRotation * tempUnk6) - (tempUnk2 * thisv->sinRotation)) + actorPosition->z;

        func_800287AC(globalCtx, &position, &sVelocity, &sAcceleration, D_808B5EB0[index][4], D_808B5EB0[index][5],
                      D_808B5EB0[index][6]);

        thisv->unk_158 += 1;
    }
}

void BgSpot16Bombstone_SpawnFragments(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    f32 velocityYMultiplier = 1.3f;
    Vec3f pos;
    Vec3f velocity;
    s32 index;
    s16 scale;

    if (thisv->actor.params == 0) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BG_SPOT16_BOMBSTONE, thisv->actor.world.pos.x,
                    thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 5);
        index = 3;
    } else {
        index = 0;
    }

    if (index < ARRAY_COUNT(D_808B6074)) {
        do {
            pos.x = ((Rand_ZeroOne() - 0.5f) * 8.0f) + thisv->actor.world.pos.x;
            pos.y = ((Rand_ZeroOne() * 5.0f) + thisv->actor.world.pos.y) + 8.0f;
            pos.z = ((Rand_ZeroOne() - 0.5f) * 8.0f) + thisv->actor.world.pos.z;

            velocity.x = (Rand_ZeroOne() - 0.5f) * 16.0f;
            velocity.y = (Rand_ZeroOne() * 14.0) + (fabsf(thisv->actor.velocity.y) * velocityYMultiplier);
            velocity.z = (Rand_ZeroOne() - 0.5f) * 16.0f;

            scale = D_808B6074[index] * thisv->actor.scale.x * 3;

            EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &thisv->actor.world.pos, -420, 0x31, 0xF, 0xF, 0, scale, 2,
                                 0x40, 160, KAKERA_COLOR_NONE, OBJECT_BOMBIWA, object_bombiwa_DL_0009E0);
            index += 1;
        } while (index != ARRAY_COUNT(D_808B6074));
    }
}

void func_808B561C(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    s32 index;
    PosRot* world;

    world = &thisv->actor.world;
    for (index = 0; index < ARRAY_COUNT(D_808B6088); index++) {
        if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BG_SPOT16_BOMBSTONE, world->pos.x, world->pos.y,
                        world->pos.z, 0, 0, 0, D_808B6088[index]) == NULL) {
            break;
        }
    }
}

void func_808B56BC(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 sinValue;
    s16 adjustedYawDiff;
    s32 yawDiff;
    s32 absYawDiff;

    if (thisv->actor.xzDistToPlayer < 130.0f && thisv->actor.yDistToPlayer < 160.0f &&
        thisv->actor.yDistToPlayer >= -10.0f) {
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        absYawDiff = ABS(yawDiff);

        adjustedYawDiff = absYawDiff - 0x3FFF;

        if (adjustedYawDiff > 0) {
            sinValue = Math_SinS(adjustedYawDiff) * thisv->actor.xzDistToPlayer;

            if (sinValue >= 0.0f) {
                player->actor.world.pos.x += sinValue * thisv->sinRotation;
                player->actor.world.pos.z += sinValue * thisv->cosRotation;
            } else {
                osSyncPrintf("Error 補正出来ない(%s %d)(arg_data 0x%04x)(hosei_angY %x)\n",
                             "../z_bg_spot16_bombstone.c", 935, thisv->actor.params, adjustedYawDiff);
            }
        }
    }
}

void func_808B57E0(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    Actor* playerHeldActor;
    Player* player = GET_PLAYER(globalCtx);
    EnBombf* currentBomb;

    if (sTimer > 0) {
        sTimer--;
    }

    if (sPlayerBomb != NULL) {
        if (sPlayerBomb->actor.update == NULL) {
            sPlayerBomb = NULL;
        } else if (sTimer <= 0 && sPlayerBomb->actor.world.pos.y < 1400.0f &&
                   Math3D_Dist1DSq(sPlayerBomb->actor.world.pos.x + 1579.0f, sPlayerBomb->actor.world.pos.z + 790.0f) <
                       SQ(400.0f) &&
                   sPlayerBomb->actor.params == 0) {
            currentBomb = sPlayerBomb;
            if (currentBomb->timer > 0) {
                sTimer = currentBomb->timer + 20;
                OnePointCutscene_Init(globalCtx, 4180, sTimer, NULL, MAIN_CAM);
            }
        }
    } else if (player->stateFlags1 & 0x800) {
        playerHeldActor = player->heldActor;
        if (playerHeldActor != NULL && playerHeldActor->category == ACTORCAT_EXPLOSIVE &&
            playerHeldActor->id == ACTOR_EN_BOMBF) {
            sPlayerBomb = (EnBombf*)playerHeldActor;
        }
    }
}

void func_808B5934(BgSpot16Bombstone* thisv) {
    thisv->actor.draw = BgSpot16Bombstone_Draw;
    thisv->actionFunc = func_808B5950;
}

void func_808B5950(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    s32 pad;

    func_808B56BC(thisv, globalCtx);
    func_808B57E0(thisv, globalCtx);

    if (globalCtx) {}

    if (thisv->colliderCylinder.base.acFlags & AC_HIT) {
        thisv->colliderCylinder.base.acFlags &= ~AC_HIT;

        func_808B561C(thisv, globalCtx);

        OnePointCutscene_Init(globalCtx, 4180, 50, NULL, MAIN_CAM);

        Flags_SetSwitch(globalCtx, thisv->switchFlag);
        gSaveContext.eventChkInf[2] |= 8;

        func_808B5A78(thisv);
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCylinder.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
    }

    if (mREG(64) == 1) {
        func_808B561C(thisv, globalCtx);
        mREG(64) = -10;
    } else if (mREG(64) < 0) {
        mREG(64)++;
    }
}

void func_808B5A78(BgSpot16Bombstone* thisv) {
    thisv->unk_154 = 0;
    thisv->unk_158 = 0;
    thisv->actor.draw = NULL;
    thisv->actionFunc = func_808B5A94;
}

void func_808B5A94(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {

    func_808B5240(thisv, globalCtx);

    if (thisv->unk_154 == 56) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
    }

    if (thisv->unk_154 > 60) {
        Actor_Kill(&thisv->actor);
    }
}

void func_808B5AF0(BgSpot16Bombstone* thisv) {
    thisv->actionFunc = func_808B5B04;
    thisv->actor.draw = NULL;
}

void func_808B5B04(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->bombiwaBankIndex)) {
        func_808B5B58(thisv);
        thisv->actor.draw = BgSpot16Bombstone_Draw;
    }
}

void func_808B5B58(BgSpot16Bombstone* thisv) {
    thisv->unk_154 = 0;
    thisv->actionFunc = func_808B5B6C;
}

void func_808B5B6C(BgSpot16Bombstone* thisv, GlobalContext* globalCtx) {
    Actor* actor = &thisv->actor;

    Actor_MoveForward(actor);
    actor->shape.rot.x += thisv->unk_210;
    actor->shape.rot.z += thisv->unk_212;

    if (thisv->unk_154 > 60) {
        Actor_Kill(actor);
        return;
    }

    if (actor->bgCheckFlags & 8 || (actor->bgCheckFlags & 1 && actor->velocity.y < 0.0f)) {
        BgSpot16Bombstone_SpawnFragments(thisv, globalCtx);
        BgSpot16Bombstone_SpawnDust(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &actor->world.pos, 20, NA_SE_EV_ROCK_BROKEN);
        Actor_Kill(actor);
        return;
    }

    Actor_UpdateBgCheckInfo(globalCtx, actor, 17.5f, 35.0f, 0.0f, 5);
}

void BgSpot16Bombstone_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Bombstone* thisv = (BgSpot16Bombstone*)thisx;

    thisv->unk_154++;
    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgSpot16Bombstone_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Bombstone* thisv = (BgSpot16Bombstone*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot16_bombstone.c", 1253);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_spot16_bombstone.c", 1257),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (thisv->actor.params == 0xFF) {
        // The boulder is intact
        gSPDisplayList(POLY_OPA_DISP++, thisv->dList);
    } else {
        // The boulder is debris
        gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[thisv->bombiwaBankIndex].segment);
        gSPDisplayList(POLY_OPA_DISP++, thisv->dList);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot16_bombstone.c", 1274);
}

void BgSpot16Bombstone_Reset(void) {
    sPlayerBomb = NULL;
    sTimer = 0;
}