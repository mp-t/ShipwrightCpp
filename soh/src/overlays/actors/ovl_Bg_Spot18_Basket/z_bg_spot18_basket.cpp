#include "z_bg_spot18_basket.h"
#include "objects/object_spot18_obj/object_spot18_obj.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void BgSpot18Basket_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Basket_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Basket_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Basket_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Basket_Reset(void);

void func_808B7BCC(BgSpot18Basket* thisv, GlobalContext* globalCtx);
void func_808B7AEC(BgSpot18Basket* thisv);
void func_808B7B58(BgSpot18Basket* thisv);
void func_808B7BB0(BgSpot18Basket* thisv);
void func_808B7D38(BgSpot18Basket* thisv);
void func_808B7F74(BgSpot18Basket* thisv);
void func_808B818C(BgSpot18Basket* thisv);
void func_808B7AFC(BgSpot18Basket* thisv, GlobalContext* globalCtx);
void func_808B7B6C(BgSpot18Basket* thisv, GlobalContext* globalCtx);
void func_808B7D50(BgSpot18Basket* thisv, GlobalContext* globalCtx);
void func_808B7FC0(BgSpot18Basket* thisv, GlobalContext* globalCtx);
void func_808B81A0(BgSpot18Basket* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Spot18_Basket_InitVars = {
    ACTOR_BG_SPOT18_BASKET,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SPOT18_OBJ,
    sizeof(BgSpot18Basket),
    (ActorFunc)BgSpot18Basket_Init,
    (ActorFunc)BgSpot18Basket_Destroy,
    (ActorFunc)BgSpot18Basket_Update,
    (ActorFunc)BgSpot18Basket_Draw,
    (ActorResetFunc)BgSpot18Basket_Reset,
};

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 2040, 0 }, 54 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 1, { { 0, 1400, 0 }, 13 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    2,
    sJntSphElementsInit,
};

static s16 D_808B85C8[] = { 0x8000, 0x2AAA, 0xD555, 0x0000 };

void func_808B7710(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Basket* thisv = (BgSpot18Basket*)thisx;

    Collider_InitJntSph(globalCtx, &thisv->colliderJntSph);
    Collider_SetJntSph(globalCtx, &thisv->colliderJntSph, &thisv->dyna.actor, &sJntSphInit, thisv->ColliderJntSphElements);
    thisv->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
}

static s16 D_808B85D0 = 0;
void func_808B7770(BgSpot18Basket* thisv, GlobalContext* globalCtx, f32 arg2) {
    Vec3f acceleration;
    Vec3f velocity;
    Vec3f position;
    f32 cosValue;
    s32 i;
    f32 randomValue;
    f32 sinValue;
    s32 count;

    for (i = 0, count = 2; i != count; i++) {
        if (globalCtx) {}
        if (!(arg2 < Rand_ZeroOne())) {
            D_808B85D0 += 0x7530;

            sinValue = Math_SinS(D_808B85D0);
            cosValue = Math_CosS(D_808B85D0);

            randomValue = (Rand_ZeroOne() * 35.0f) + 35.0f;

            position.x = (randomValue * sinValue) + thisv->dyna.actor.world.pos.x;
            position.y = thisv->dyna.actor.world.pos.y + 10.0f;
            position.z = (randomValue * cosValue) + thisv->dyna.actor.world.pos.z;

            velocity.x = sinValue;
            velocity.y = 0.0f;
            velocity.z = cosValue;

            acceleration.x = 0.0f;
            acceleration.y = 0.5f;
            acceleration.z = 0.0f;

            func_800286CC(globalCtx, &position, &velocity, &acceleration, ((Rand_ZeroOne() * 16) + 80),
                          ((Rand_ZeroOne() * 30) + 80));
        }
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgSpot18Basket_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSpot18Basket* thisv = (BgSpot18Basket*)thisx;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, DPM_UNK3);
    func_808B7710(&thisv->dyna.actor, globalCtx);
    CollisionHeader_GetVirtual(&gGoronCityVaseCol, &colHeader);

    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    ActorShape_Init(&thisv->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 15.0f);
    thisv->dyna.actor.home.pos.y += 0.01f;
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y;

    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
        func_808B7BB0(thisv);
        return;
    }

    func_808B7AEC(thisv);
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_BG_SPOT18_FUTA,
                       thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z,
                       thisv->dyna.actor.shape.rot.x, thisv->dyna.actor.shape.rot.y + 0x1555,
                       thisv->dyna.actor.shape.rot.z, -1);

    if (thisv->dyna.actor.child == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Ｅｒｒｏｒ : 変化壷蓋発生失敗(%s %d)\n", "../z_bg_spot18_basket.c", 351);
        osSyncPrintf(VT_RST);
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgSpot18Basket_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Basket* thisv = (BgSpot18Basket*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyJntSph(globalCtx, &thisv->colliderJntSph);
}

void func_808B7AEC(BgSpot18Basket* thisv) {
    thisv->actionFunc = func_808B7AFC;
}

void func_808B7AFC(BgSpot18Basket* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
        OnePointCutscene_Init(globalCtx, 4220, 80, &thisv->dyna.actor, MAIN_CAM);
        func_808B7B58(thisv);
    }
}

void func_808B7B58(BgSpot18Basket* thisv) {
    thisv->actionFunc = func_808B7B6C;
    thisv->unk_216 = 0;
}

void func_808B7B6C(BgSpot18Basket* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_216 > 20) {
        func_808B7BB0(thisv);
        thisv->dyna.actor.child->parent = NULL;
        thisv->dyna.actor.child = NULL;
    }
}

void func_808B7BB0(BgSpot18Basket* thisv) {
    thisv->actionFunc = func_808B7BCC;
    thisv->unk_210 = thisv->unk_20C = 0;
}

void func_808B7BCC(BgSpot18Basket* thisv, GlobalContext* globalCtx) {
    f32 positionDiff;
    Actor* colliderBaseAc;

    Math_StepToS(&thisv->unk_210, 0x1F4, 0x1E);

    thisv->dyna.actor.shape.rot.y += thisv->unk_210;

    Math_StepToF(&thisv->unk_208, 50.0f, 1.5f);
    Math_StepToS(&thisv->unk_20C, 400, 15);

    thisv->unk_20E += thisv->unk_20C;

    thisv->dyna.actor.world.pos.x = (Math_SinS(thisv->unk_20E) * thisv->unk_208) + thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z = (Math_CosS(thisv->unk_20E) * thisv->unk_208) + thisv->dyna.actor.home.pos.z;

    if (thisv->colliderJntSph.base.acFlags & AC_HIT) {
        colliderBaseAc = thisv->colliderJntSph.base.ac;

        if (colliderBaseAc != NULL) {
            positionDiff = colliderBaseAc->world.pos.y - thisv->dyna.actor.world.pos.y;

            if (positionDiff > 120.0f && positionDiff < 200.0f) {
                if (Math3D_Dist2DSq(colliderBaseAc->world.pos.z, thisv->colliderJntSph.base.ac->world.pos.x,
                                    thisv->dyna.actor.world.pos.z, thisv->dyna.actor.world.pos.x) < SQ(32.0f)) {
                    OnePointCutscene_Init(globalCtx, 4210, 240, &thisv->dyna.actor, MAIN_CAM);
                    func_808B7D38(thisv);
                    func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
                }
            }
        }
    }
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void func_808B7D38(BgSpot18Basket* thisv) {
    thisv->actionFunc = func_808B7D50;
    thisv->unk_216 = 0;
    thisv->unk_214 = 0;
}

void func_808B7D50(BgSpot18Basket* thisv, GlobalContext* globalCtx) {
    f32 tempValue2;
    f32 tempValue;

    if (thisv->unk_216 > 120) {
        Math_StepToS(&thisv->unk_210, 0x3E8, 0x32);
    } else {
        Math_StepToS(&thisv->unk_210, 0xBB8, 0x64);
    }

    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.shape.rot.y + thisv->unk_210;

    if (thisv->unk_216 < 70) {
        Math_StepToF(&thisv->unk_208, 100.0f, 2.0f);
    } else {
        Math_StepToF(&thisv->unk_208, 0.0f, 2.0f);
    }

    Math_StepToS(&thisv->unk_20C, 1000, 20);

    thisv->unk_20E += thisv->unk_20C;

    thisv->dyna.actor.world.pos.x = (Math_SinS(thisv->unk_20E) * thisv->unk_208) + thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z = (Math_CosS(thisv->unk_20E) * thisv->unk_208) + thisv->dyna.actor.home.pos.z;

    thisv->unk_212 += 0xBB8;

    Math_StepToS(&thisv->unk_214, 0x5DC, 0x1E);

    thisv->dyna.actor.shape.rot.x = Math_CosS(thisv->unk_212) * thisv->unk_214;
    thisv->dyna.actor.shape.rot.z = -Math_SinS(thisv->unk_212) * thisv->unk_214;

    if (thisv->unk_216 > 140) {
        func_808B7F74(thisv);
    }

    if (thisv->unk_216 < 80) {
        func_808B7770(thisv, globalCtx, 1.0f);
    } else {
        func_808B7770(thisv, globalCtx, 0.8f);
    }

    tempValue2 = (thisv->unk_210 - 500) * 0.0006f;

    tempValue = CLAMP(tempValue2, 0.0f, 1.5f);

    func_800F436C(&thisv->dyna.actor.projectedPos, NA_SE_EV_WALL_MOVE_SP - SFX_FLAG, tempValue);
}

void func_808B7F74(BgSpot18Basket* thisv) {
    s16 shapeRotY;

    shapeRotY = thisv->dyna.actor.shape.rot.y;
    thisv->actionFunc = func_808B7FC0;

    if ((shapeRotY < -0x2E93) || (shapeRotY >= 0x7C19)) {
        thisv->unk_218 = 2;
    } else if (shapeRotY < 0x26C2) {
        thisv->unk_218 = 1;
    } else {
        thisv->unk_218 = 0;
    }

    thisv->unk_216 = 0;
}

void func_808B7FC0(BgSpot18Basket* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 tempUnk214;
    f32 tempUnk210;
    s16 arrayValue;
    f32 clampedTempUnk210;

    thisv->unk_212 += 0xBB8;

    if (thisv->unk_216 >= 13) {
        tempUnk214 = Math_StepToS(&thisv->unk_214, 0, 55);
    } else {
        tempUnk214 = 0;
    }

    thisv->dyna.actor.shape.rot.x = Math_CosS(thisv->unk_212) * thisv->unk_214;
    thisv->dyna.actor.shape.rot.z = -Math_SinS(thisv->unk_212) * thisv->unk_214;

    Math_StepToS(&thisv->unk_210, 0x1F4, 0xA);
    thisv->dyna.actor.shape.rot.y += thisv->unk_210;

    if (tempUnk214 != 0) {
        arrayValue = D_808B85C8[thisv->unk_218];

        if ((s16)(thisv->dyna.actor.shape.rot.y - arrayValue) >= 0) {
            thisv->dyna.actor.shape.rot.y = arrayValue;

            func_808B818C(thisv);
            func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        }
    }

    if (thisv->unk_216 < 30) {
        func_808B7770(thisv, globalCtx, 0.5f);
    } else {
        func_808B7770(thisv, globalCtx, 0.3f);
    }

    tempUnk210 = (thisv->unk_210 - 500) * 0.0006f;

    clampedTempUnk210 = CLAMP(tempUnk210, 0.0f, 1.5f);

    func_800F436C(&thisv->dyna.actor.projectedPos, NA_SE_EV_WALL_MOVE_SP - SFX_FLAG, clampedTempUnk210);
}

void func_808B818C(BgSpot18Basket* thisv) {
    thisv->actionFunc = func_808B81A0;
    thisv->unk_216 = 0;
}

static s16 D_808B85E4[] = { -0x0FA0, 0x0320, 0x0FA0 };

void func_808B81A0(BgSpot18Basket* thisv, GlobalContext* globalCtx) {
    s32 i;
    Actor* actor = &thisv->dyna.actor;
    Vec3f tempVector;
    EnItem00* collectible;

    if (thisv->unk_216 == 1) {
        tempVector.x = actor->world.pos.x;
        tempVector.y = actor->world.pos.y + 170.0f;
        tempVector.z = actor->world.pos.z;

        if (thisv->unk_218 == 0) {
            for (i = 0; i < ARRAY_COUNT(D_808B85E4); i++) {
                collectible = Item_DropCollectible(globalCtx, &tempVector, ITEM00_BOMBS_A);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = D_808B85E4[i];
                }
            }
        } else if (thisv->unk_218 == 1) {
            for (i = 0; i < ARRAY_COUNT(D_808B85E4); i++) {
                collectible = Item_DropCollectible(globalCtx, &tempVector, ITEM00_RUPEE_GREEN);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = D_808B85E4[i];
                }
            }
        } else if (thisv->unk_218 == 2) {
            if ((thisv->unk_21A != 0) || Flags_GetCollectible(globalCtx, (actor->params & 0x3F))) {
                collectible = Item_DropCollectible(globalCtx, &tempVector, ITEM00_RUPEE_PURPLE);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = D_808B85E4[1];
                }
            } else {
                collectible =
                    Item_DropCollectible(globalCtx, &tempVector, ((actor->params & 0x3F) << 8) | ITEM00_HEART_PIECE);
                if (collectible != NULL) {
                    collectible->actor.velocity.y = 11.0f;
                    collectible->actor.world.rot.y = D_808B85E4[1];
                    thisv->unk_21A = 1;
                }
            }

            collectible = Item_DropCollectible(globalCtx, &tempVector, ITEM00_RUPEE_RED);
            if (collectible != NULL) {
                collectible->actor.velocity.y = 11.0f;
                collectible->actor.world.rot.y = D_808B85E4[0];
            }

            collectible = Item_DropCollectible(globalCtx, &tempVector, ITEM00_RUPEE_BLUE);
            if (collectible != NULL) {
                collectible->actor.velocity.y = 11.0f;
                collectible->actor.world.rot.y = D_808B85E4[2];
            }
        }
    } else if (thisv->unk_216 == 2) {
        if (thisv->unk_218 == 2) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        } else {
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        }
    } else if (thisv->unk_216 == 200) {
        func_808B7BB0(thisv);
    }
}

void BgSpot18Basket_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSpot18Basket* thisv = (BgSpot18Basket*)thisx;
    s32 bgId;

    thisv->unk_216++;
    thisv->actionFunc(thisv, globalCtx);
    thisv->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->dyna.actor.floorPoly, &bgId,
                                                               &thisv->dyna.actor, &thisv->dyna.actor.world.pos);
    if (thisv->actionFunc != func_808B7AFC) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
        if (thisv->actionFunc != func_808B7B6C) {
            thisv->colliderJntSph.base.acFlags &= ~AC_HIT;
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
        }
    }
}

void BgSpot18Basket_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Basket* thisv = (BgSpot18Basket*)thisx;

    Collider_UpdateSpheres(0, &thisv->colliderJntSph);
    Collider_UpdateSpheres(1, &thisv->colliderJntSph);
    Gfx_DrawDListOpa(globalCtx, gGoronCityVaseDL);
}

void BgSpot18Basket_Reset(void) {
    D_808B85D0 = 0;
}