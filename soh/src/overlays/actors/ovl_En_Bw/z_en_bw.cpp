/*
 * File: z_en_bw.c
 * Overlay: ovl_En_Bw
 * Description: Torch slug
 */

#include "z_en_bw.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_bw/object_bw.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnBw_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBw_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBw_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBw_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnBw_Reset(void);

void func_809CE884(EnBw* thisv, GlobalContext* globalCtx);
void func_809CE9A8(EnBw* thisv);
void func_809CEA24(EnBw* thisv, GlobalContext* globalCtx);
void func_809CF72C(EnBw* thisv);
void func_809CF7AC(EnBw* thisv, GlobalContext* globalCtx);
void func_809CF8F0(EnBw* thisv);
void func_809CF984(EnBw* thisv, GlobalContext* globalCtx);
void func_809CFBA8(EnBw* thisv);
void func_809CFC4C(EnBw* thisv, GlobalContext* globalCtx);
void func_809CFF10(EnBw* thisv);
void func_809CFF98(EnBw* thisv, GlobalContext* globalCtx);
void func_809D00F4(EnBw* thisv);
void func_809D014C(EnBw* thisv, GlobalContext* globalCtx);
void func_809D01CC(EnBw* thisv);
void func_809D0268(EnBw* thisv, GlobalContext* globalCtx);
void func_809D03CC(EnBw* thisv);
void func_809D0424(EnBw* thisv, GlobalContext* globalCtx);

const ActorInit En_Bw_InitVars = {
    ACTOR_EN_BW,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BW,
    sizeof(EnBw),
    (ActorFunc)EnBw_Init,
    (ActorFunc)EnBw_Destroy,
    (ActorFunc)EnBw_Update,
    (ActorFunc)EnBw_Draw,
    (ActorResetFunc)EnBw_Reset,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 30, 65, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 35, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(2, 0xF),
    /* Ice arrow     */ DMG_ENTRY(4, 0xE),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xF),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0xE),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 sSlugGroup = 0;

void EnBw_SetupAction(EnBw* thisv, EnBwActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnBw_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBw* thisv = (EnBw*)thisx;

    Actor_SetScale(&thisv->actor, 0.012999999f);
    thisv->actor.naviEnemyId = 0x23;
    thisv->actor.gravity = -2.0f;
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_bw_Skel_0020F0, &object_bw_Anim_000228, thisv->jointTable,
                   thisv->morphTable, 12);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    thisv->actor.colChkInfo.health = 6;
    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisv->actor.focus.pos = thisv->actor.world.pos;
    func_809CE9A8(thisv);
    thisv->color1.a = thisv->color1.r = 255;
    thisv->color1.g = thisv->color1.b = 0;
    thisv->unk_248 = 0.6f;
    thisv->unk_221 = 3;
    Collider_InitCylinder(globalCtx, &thisv->collider1);
    //! thisv->collider2 should have Init called on it, but it doesn't matter since the heap is zeroed before use.
    Collider_SetCylinder(globalCtx, &thisv->collider1, &thisv->actor, &sCylinderInit1);
    Collider_SetCylinder(globalCtx, &thisv->collider2, &thisv->actor, &sCylinderInit2);
    thisv->unk_236 = thisv->actor.world.rot.y;
    thisv->actor.params = sSlugGroup;
    sSlugGroup = (sSlugGroup + 1) & 3;
}

void EnBw_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnBw* thisv = (EnBw*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider1);
    Collider_DestroyCylinder(globalCtx, &thisv->collider2);
}

void func_809CE884(EnBw* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    thisv->unk_222 -= 250;
    thisv->actor.scale.x = 0.013f + Math_SinF(thisv->unk_222 * 0.001f) * 0.0069999998f;
    thisv->actor.scale.y = 0.013f - Math_SinF(thisv->unk_222 * 0.001f) * 0.0069999998f;
    thisv->actor.scale.z = 0.013f + Math_SinF(thisv->unk_222 * 0.001f) * 0.0069999998f;
    if (thisv->unk_222 == 0) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        func_809CE9A8(thisv);
    }
}

void func_809CE9A8(EnBw* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_bw_Anim_000228, -2.0f);
    thisv->unk_220 = 2;
    thisv->unk_222 = Rand_ZeroOne() * 200.0f + 200.0f;
    thisv->unk_232 = 0;
    thisv->actor.speedXZ = 0.0f;
    EnBw_SetupAction(thisv, func_809CEA24);
}

void func_809CEA24(EnBw* thisv, GlobalContext* globalCtx) {
    CollisionPoly* sp74 = NULL;
    Vec3f sp68;
    u32 sp64 = 0;
    s16 sp62;
    s16 sp60;
    f32 sp5C;
    f32 sp58;
    Player* player = GET_PLAYER(globalCtx);
    Player* player2 = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    thisv->unk_244 = thisv->unk_250 + 0.1f;
    sp58 = Math_CosF(thisv->unk_240);
    thisv->unk_240 += thisv->unk_244;
    if (thisv->unk_24C < 0.8f) {
        thisv->unk_24C += 0.1f;
    }
    thisv->actor.scale.x = 0.013f - Math_SinF(thisv->unk_240) * (thisv->unk_24C * 0.004f);
    thisv->actor.scale.y = 0.013f - Math_SinF(thisv->unk_240) * (thisv->unk_24C * 0.004f);
    thisv->actor.scale.z = 0.013f + Math_SinF(thisv->unk_240) * (thisv->unk_24C * 0.004f);
    sp5C = Math_CosF(thisv->unk_240);
    if (thisv->unk_232 == 0) {
        if (ABS(sp58) < ABS(sp5C)) {
            thisv->unk_232++;
        }
    } else {
        if (ABS(sp58) > ABS(sp5C)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLEWALK_WALK);
            thisv->unk_232 = 0;
        }
    }
    sp5C *= thisv->unk_24C * (10.0f * thisv->unk_244);
    thisv->actor.speedXZ = ABS(sp5C);
    if (thisv->unk_221 != 1) {
        sp58 = Math_SinF(thisv->unk_240);
        sp60 = ABS(sp58) * 85.0f;
        thisv->color1.g = sp60;
    }
    if ((((globalCtx->gameplayFrames % 4) == (u32)thisv->actor.params) && (thisv->actor.speedXZ != 0.0f) &&
         (sp64 = BgCheck_AnyLineTest2(&globalCtx->colCtx, &thisv->actor.world.pos, &thisv->unk_264, &sp68, &sp74, 1, 0, 0,
                                      1))) ||
        (thisv->unk_222 == 0)) {
        if (sp74 != NULL) {
            sp74 = SEGMENTED_TO_VIRTUAL(sp74);
            sp62 = Math_FAtan2F(sp74->normal.x, sp74->normal.z) * ((f32)0x8000 / std::numbers::pi_v<float>);
        } else {
            sp62 = thisv->actor.world.rot.y + 0x8000;
        }
        if ((thisv->unk_236 != sp62) || (sp64 == 0)) {
            if (BgCheck_AnyLineTest2(&globalCtx->colCtx, &thisv->unk_270, &thisv->unk_288, &sp68, &sp74, 1, 0, 0, 1)) {
                sp64 |= 2;
            }
            if (BgCheck_AnyLineTest2(&globalCtx->colCtx, &thisv->unk_270, &thisv->unk_27C, &sp68, &sp74, 1, 0, 0, 1)) {
                sp64 |= 4;
            }
            switch (sp64) {
                case 0:
                    thisv->unk_236 += thisv->unk_238;
                case 1:
                    if (thisv->unk_221 == 3) {
                        if (globalCtx->gameplayFrames & 2) {
                            thisv->unk_238 = 0x4000;
                        } else {
                            thisv->unk_238 = -0x4000;
                        }
                    } else {
                        if ((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y) >= 0.0f) {
                            thisv->unk_238 = 0x4000;
                        } else {
                            thisv->unk_238 = -0x4000;
                        }
                        if (thisv->unk_221 == 1) {
                            thisv->unk_238 = -thisv->unk_238;
                        }
                    }
                    break;
                case 2:
                    thisv->unk_236 += thisv->unk_238;
                case 3:
                    thisv->unk_238 = 0x4000;
                    break;
                case 4:
                    thisv->unk_236 += thisv->unk_238;
                case 5:
                    thisv->unk_238 = -0x4000;
                    break;
                case 7:
                    thisv->unk_238 = 0;
                    break;
            }
            if (sp64 != 6) {
                thisv->unk_236 = sp62;
            }
            thisv->unk_222 = (Rand_ZeroOne() * 200.0f) + 200.0f;
        }
    } else if ((thisv->actor.speedXZ != 0.0f) && (thisv->actor.bgCheckFlags & 8)) {
        if (thisv->unk_236 != thisv->actor.wallYaw) {
            sp64 = 1;
            thisv->unk_236 = thisv->actor.wallYaw;
            if (thisv->unk_221 == 3) {
                if (globalCtx->gameplayFrames & 0x20) {
                    thisv->unk_238 = 0x4000;
                } else {
                    thisv->unk_238 = -0x4000;
                }
                thisv->actor.bgCheckFlags &= ~8;
                thisv->unk_222 = (Rand_ZeroOne() * 20.0f) + 160.0f;
            } else {
                if ((s16)(thisv->actor.yawTowardsPlayer - thisv->unk_236) >= 0) {
                    thisv->unk_238 = 0x4000;
                } else {
                    thisv->unk_238 = -0x4000;
                }
                if (thisv->unk_221 == 1) {
                    thisv->unk_238 = -thisv->unk_238;
                }
            }
        } else if (thisv->unk_221 == 0) {
            sp64 = BgCheck_AnyLineTest2(&globalCtx->colCtx, &thisv->actor.world.pos, &player->actor.world.pos, &sp68,
                                        &sp74, 1, 0, 0, 1);
            if (sp64 != 0) {
                sp74 = SEGMENTED_TO_VIRTUAL(sp74);
                sp60 = Math_FAtan2F(sp74->normal.x, sp74->normal.z) * ((f32)0x8000 / std::numbers::pi_v<float>);
                if (thisv->unk_236 != sp60) {
                    if ((s16)(thisv->actor.yawTowardsPlayer - sp60) >= 0) {
                        thisv->unk_238 = 0x4000;
                    } else {
                        thisv->unk_238 = -0x4000;
                    }
                    thisv->unk_236 = sp60;
                }
            }
        }
    }
    thisv->unk_222--;
    if (thisv->unk_224 != 0) {
        thisv->unk_224--;
    }
    if ((thisv->unk_234 == 0) &&
        !Actor_TestFloorInDirection(&thisv->actor, globalCtx, 50.0f, thisv->unk_236 + thisv->unk_238)) {
        if (thisv->unk_238 != 0x4000) {
            thisv->unk_238 = 0x4000;
        } else {
            thisv->unk_238 = -0x4000;
        }
    }
    switch (thisv->unk_221) {
        case 3:
            Math_SmoothStepToF(&thisv->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
            if ((thisv->unk_224 == 0) && (thisv->actor.xzDistToPlayer < 200.0f) &&
                (ABS(thisv->actor.yDistToPlayer) < 50.0f) && Actor_IsFacingPlayer(&thisv->actor, 0x1C70)) {
                func_809CF72C(thisv);
            } else {
                Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->unk_236 + thisv->unk_238, 1,
                                   thisv->actor.speedXZ * 1000.0f, 0);
            }
            break;
        case 0:
            Math_SmoothStepToF(&thisv->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
            if (sp64 == 0) {
                Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1,
                                   thisv->actor.speedXZ * 1000.0f, 0);
                if ((thisv->actor.xzDistToPlayer < 90.0f) && (thisv->actor.yDistToPlayer < 50.0f) &&
                    Actor_IsFacingPlayer(&thisv->actor, 0x1554) &&
                    Actor_TestFloorInDirection(&thisv->actor, globalCtx, 71.24802f, thisv->actor.yawTowardsPlayer)) {
                    func_809CF8F0(thisv);
                }
            } else {
                Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->unk_236 + thisv->unk_238, 1,
                                   thisv->actor.speedXZ * 1000.0f, 0);
            }
            if ((thisv->unk_224 == 0) || (ABS(thisv->actor.yDistToPlayer) > 60.0f) || (player2->stateFlags1 & 0x6000)) {
                thisv->unk_221 = 3;
                thisv->unk_224 = 150;
                thisv->unk_250 = 0.0f;
            }
            break;
        case 1:
            if (((sp64 == 0) && !(thisv->actor.bgCheckFlags & 8)) || Actor_IsFacingPlayer(&thisv->actor, 0x1C70)) {
                if (Actor_IsFacingPlayer(&thisv->actor, 0x1C70)) {
                    thisv->unk_238 = -thisv->unk_238;
                }
                Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer - 0x8000, 1,
                                   thisv->actor.speedXZ * 1000.0f, 0);
            } else {
                Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->unk_236 + thisv->unk_238, 1,
                                   thisv->actor.speedXZ * 1000.0f, 0);
            }
            if (thisv->unk_224 <= 200) {
                sp60 = Math_SinS(thisv->unk_224 * (0x960 - thisv->unk_224)) * 55.0f;
                thisv->color1.r = 255 - ABS(sp60);
                sp60 = Math_SinS(thisv->unk_224 * (0x960 - thisv->unk_224)) * 115.0f;
                thisv->color1.g = ABS(sp60) + 85;
                sp60 = Math_SinS(0x960 - thisv->unk_224) * 255.0f;
                thisv->color1.b = ABS(sp60);
                if (thisv->unk_224 == 0) {
                    thisv->unk_221 = 3;
                    thisv->unk_250 = 0.0f;
                }
            }
            break;
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void func_809CF72C(EnBw* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_bw_Anim_0021A0, -2.0f);
    thisv->unk_220 = 3;
    thisv->unk_221 = 0;
    thisv->unk_250 = 0.6f;
    thisv->unk_222 = 20;
    thisv->unk_224 = 0xBB8;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLEWALK_AIM);
    EnBw_SetupAction(thisv, func_809CF7AC);
}

void func_809CF7AC(EnBw* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->unk_222 > 0) {
            thisv->unk_222--;
        } else {
            thisv->unk_222 = 100;
        }
    }
    if (thisv->unk_222 >= 20) {
        thisv->unk_224 -= 250;
    }
    thisv->actor.scale.x = 0.013f - Math_SinF(thisv->unk_224 * 0.001f) * 0.0034999999f;
    thisv->actor.scale.y = 0.013f + Math_SinF(thisv->unk_224 * 0.001f) * 0.0245f;
    thisv->actor.scale.z = 0.013f - Math_SinF(thisv->unk_224 * 0.001f) * 0.0034999999f;
    if (thisv->unk_224 == 0) {
        func_809CE9A8(thisv);
        thisv->unk_224 = 200;
    }
}

void func_809CF8F0(EnBw* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_bw_Anim_002250, -1.0f);
    thisv->actor.speedXZ = 7.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->unk_220 = 4;
    thisv->unk_222 = 1000;
    thisv->actor.velocity.y = 11.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->actor.flags |= ACTOR_FLAG_24;
    EnBw_SetupAction(thisv, func_809CF984);
}

void func_809CF984(EnBw* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 floorPolyType;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    thisv->unk_222 += 250;
    thisv->actor.scale.x = 0.013f - Math_SinF(thisv->unk_222 * 0.001f) * 0.0034999999f;
    thisv->actor.scale.y = 0.013f + Math_SinF(thisv->unk_222 * 0.001f) * 0.0245f;
    thisv->actor.scale.z = 0.013f - Math_SinF(thisv->unk_222 * 0.001f) * 0.0034999999f;
    if (thisv->collider1.base.atFlags & AT_HIT) {
        thisv->collider1.base.atFlags &= ~AT_HIT;
        thisv->actor.speedXZ = -6.0f;
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        if ((&player->actor == thisv->collider1.base.at) && !(thisv->collider1.base.atFlags & AT_BOUNCED)) {
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.bgCheckFlags & 3) {
        floorPolyType = func_80041D4C(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
        if ((floorPolyType == 2) || (floorPolyType == 3) || (floorPolyType == 9)) {
            Actor_Kill(&thisv->actor);
            return;
        }
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 30.0f, 0xB, 4.0f, 0, 0, 0);
        thisv->unk_222 = 3000;
        thisv->actor.flags &= ~ACTOR_FLAG_24;
        thisv->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        EnBw_SetupAction(thisv, func_809CE884);
    }
}

void func_809CFBA8(EnBw* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_bw_Anim_002250, -1.0f);
    thisv->unk_220 = 5;
    thisv->unk_222 = 1000;
    thisv->unk_260 = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 11.0f;
    thisv->unk_25C = Rand_ZeroOne() * 0.25f + 1.0f;
    thisv->unk_224 = 0xBB8;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLEWALK_REVERSE);
    EnBw_SetupAction(thisv, func_809CFC4C);
}

void func_809CFC4C(EnBw* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, 0x7FFF, 1, 0xFA0, 0);
    Math_SmoothStepToF(&thisv->unk_248, 0.0f, 1.0f, 0.05f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.bgCheckFlags & 3) {
        if ((globalCtx->gameplayFrames % 0x80) == 0) {
            thisv->unk_25C = (Rand_ZeroOne() * 0.25f) + 0.7f;
        }
        thisv->unk_221 = 4;
        thisv->unk_258 += thisv->unk_25C;
        Math_SmoothStepToF(&thisv->unk_260, 0.075f, 1.0f, 0.005f, 0.0f);
        if (thisv->actor.bgCheckFlags & 2) {
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 30.0f, 0xB, 4.0f, 0, 0, 0);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        }
        if (thisv->unk_224 != 0) {
            thisv->unk_224 -= 250;
            thisv->actor.scale.x = 0.013f + Math_SinF(thisv->unk_224 * 0.001f) * 0.0069999998f;
            thisv->actor.scale.y = 0.013f - Math_SinF(thisv->unk_224 * 0.001f) * 0.0069999998f;
            thisv->actor.scale.z = 0.013f + Math_SinF(thisv->unk_224 * 0.001f) * 0.0069999998f;
        }
        thisv->unk_222--;
        if (thisv->unk_222 == 0) {
            func_809CFF10(thisv);
        }
    } else {
        thisv->color1.r -= 8;
        thisv->color1.g += 32;
        if (thisv->color1.r < 200) {
            thisv->color1.r = 200;
        }
        if (thisv->color1.g > 200) {
            thisv->color1.g = 200;
        }
        if (thisv->color1.b > 235) {
            thisv->color1.b = 255;
        } else {
            thisv->color1.b += 40;
        }
        if (thisv->actor.shape.yOffset < 1000.0f) {
            thisv->actor.shape.yOffset += 200.0f;
        }
    }
}

void func_809CFF10(EnBw* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_bw_Anim_002250, -1.0f);
    thisv->unk_220 = 6;
    thisv->unk_222 = 1000;
    thisv->unk_221 = 3;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 11.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLEWALK_REVERSE);
    thisv->actor.bgCheckFlags &= ~1;
    EnBw_SetupAction(thisv, func_809CFF98);
}

void func_809CFF98(EnBw* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, 0, 1, 0xFA0, 0);
    Math_SmoothStepToF(&thisv->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.bgCheckFlags & 3) {
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 30.0f, 0xB, 4.0f, 0, 0, 0);
        thisv->unk_222 = 0xBB8;
        thisv->unk_250 = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        EnBw_SetupAction(thisv, func_809CE884);
    }
    if (thisv->color1.r < 247) {
        thisv->color1.r += 8;
    } else {
        thisv->color1.r = 255;
    }
    if (thisv->color1.g < 32) {
        thisv->color1.g = 0;
    } else {
        thisv->color1.g -= 32;
    }
    if (thisv->color1.b < 40) {
        thisv->color1.b = 0;
    } else {
        thisv->color1.b -= 40;
    }
    if (thisv->actor.shape.yOffset > 0.0f) {
        thisv->actor.shape.yOffset -= 200.0f;
    }
}

void func_809D00F4(EnBw* thisv) {
    thisv->unk_220 = 0;
    thisv->unk_222 = 40;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLEWALK_DEAD);
    EnBw_SetupAction(thisv, func_809D014C);
}

void func_809D014C(EnBw* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_230) {
        thisv->actor.scale.x += 0.0002f;
        thisv->actor.scale.y -= 0.0002f;
        thisv->actor.scale.z += 0.0002f;
    }
    thisv->actor.shape.shadowAlpha = thisv->color1.a -= 6;
    thisv->unk_222--;
    if (thisv->unk_222 <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

void func_809D01CC(EnBw* thisv) {
    thisv->unk_220 = 1;
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_25C = (Rand_ZeroOne() * 0.25f) + 1.0f;
    thisv->unk_260 = 0.0f;
    if (thisv->damageEffect == 0xE) {
        thisv->iceTimer = 0x50;
    }
    thisv->unk_222 = (thisv->actor.colorFilterParams & 0x4000) ? 25 : 80;
    EnBw_SetupAction(thisv, func_809D0268);
}

void func_809D0268(EnBw* thisv, GlobalContext* globalCtx) {
    thisv->color1.r -= 1;
    thisv->color1.g += 4;
    thisv->color1.b += 5;
    if (thisv->color1.r < 200) {
        thisv->color1.r = 200;
    }
    if (thisv->color1.g > 200) {
        thisv->color1.g = 200;
    }
    if (thisv->color1.b > 230) {
        thisv->color1.b = 230;
    }
    if (thisv->actor.colorFilterParams & 0x4000) {
        if ((globalCtx->gameplayFrames % 0x80) == 0) {
            thisv->unk_25C = 0.5f + Rand_ZeroOne() * 0.25f;
        }
        thisv->unk_258 += thisv->unk_25C;
        Math_SmoothStepToF(&thisv->unk_260, 0.075f, 1.0f, 0.005f, 0.0f);
    }
    thisv->unk_222--;
    if (thisv->unk_222 == 0) {
        func_809CE9A8(thisv);
        thisv->color1.r = thisv->color1.g = 200;
        thisv->color1.b = 255;
        thisv->unk_224 = 0x258;
        thisv->unk_221 = 1;
        thisv->unk_250 = 0.7f;
        thisv->unk_236++;
    }
}

void func_809D03CC(EnBw* thisv) {
    thisv->actor.speedXZ = 0.0f;
    if (thisv->damageEffect == 0xE) {
        thisv->iceTimer = 32;
    }
    thisv->unk_23C = thisv->actor.colorFilterTimer;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnBw_SetupAction(thisv, func_809D0424);
}

void func_809D0424(EnBw* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.colorFilterTimer == 0) {
        thisv->unk_23C = 0;
        if (thisv->actor.colChkInfo.health != 0) {
            if ((thisv->unk_220 != 5) && (thisv->unk_220 != 6)) {
                func_809CE9A8(thisv);
                thisv->color1.r = thisv->color1.g = 200;
                thisv->color1.b = 255;
                thisv->unk_224 = 0x258;
                thisv->unk_221 = 1;
                thisv->unk_250 = 0.7f;
                thisv->unk_236++;
            } else if (thisv->unk_220 != 5) {
                EnBw_SetupAction(thisv, func_809CFF98);
            } else {
                func_809CFF10(thisv);
            }
        } else {
            if (func_800355E4(globalCtx, &thisv->collider2.base)) {
                thisv->unk_230 = 0;
                thisv->actor.scale.y -= 0.009f;
                Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 30.0f, 0xB, 4.0f, 0, 0, 0);
            } else {
                thisv->unk_230 = 1;
            }
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x90);
            func_809D00F4(thisv);
        }
    }
}

void func_809D0584(EnBw* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.bgCheckFlags & 0x10) && (thisv->actor.bgCheckFlags & 1)) {
        thisv->unk_230 = 0;
        thisv->actor.scale.y -= 0.009f;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 30.0f, 0xB, 4.0f, 0, 0, 0);
        func_809D00F4(thisv);
    } else {
        if (thisv->collider2.base.acFlags & AC_HIT) {
            thisv->collider2.base.acFlags &= ~AC_HIT;
            if ((thisv->actor.colChkInfo.damageEffect == 0) || (thisv->unk_220 == 6)) {
                return;
            }
            thisv->damageEffect = thisv->actor.colChkInfo.damageEffect;
            Actor_SetDropFlag(&thisv->actor, &thisv->collider2.info, 0);
            if ((thisv->damageEffect == 1) || (thisv->damageEffect == 0xE)) {
                if (thisv->unk_23C == 0) {
                    Actor_ApplyDamage(&thisv->actor);
                    Actor_SetColorFilter(&thisv->actor, 0, 0x78, 0, 0x50);
                    func_809D03CC(thisv);
                    thisv->unk_248 = 0.0f;
                }
                return;
            }
            if (thisv->unk_248 == 0.0f) {
                Actor_ApplyDamage(&thisv->actor);
            }
            if (((thisv->unk_221 == 1) || (thisv->unk_221 == 4)) && (thisv->actor.colChkInfo.health == 0)) {
                if (thisv->unk_220 != 0) {
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
                    if (func_800355E4(globalCtx, &thisv->collider2.base)) {
                        thisv->unk_230 = 0;
                        thisv->actor.scale.y -= 0.009f;
                        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 30.0f, 0xB, 4.0f, 0,
                                                 0, 0);
                    } else {
                        thisv->unk_230 = 1;
                    }
                    Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x90);
                    func_809D00F4(thisv);
                }
            } else if ((thisv->unk_220 != 1) && (thisv->unk_220 != 6)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLEWALK_DAMAGE);
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
                if (thisv->unk_220 != 5) {
                    func_809D01CC(thisv);
                }
                thisv->unk_248 = 0.0f;
            }
        }
        if ((globalCtx->actorCtx.unk_02 != 0) && (thisv->actor.xzDistToPlayer <= 400.0f) &&
            (thisv->actor.bgCheckFlags & 1)) {
            if (thisv->unk_220 == 5) {
                thisv->unk_23C = 0;
                func_809CFF10(thisv);
            } else if (thisv->unk_220 != 0) {
                thisv->unk_23C = 0;
                func_809CFBA8(thisv);
            }
        }
    }
}

void EnBw_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnBw* thisv = (EnBw*)thisx;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s32 pad[3]; // probably an unused Vec3f
    Color_RGBA8 sp50 = { 255, 200, 0, 255 };
    Color_RGBA8 sp4C = { 255, 80, 0, 255 };
    Color_RGBA8 sp48 = { 0, 0, 0, 255 };
    Color_RGBA8 sp44 = { 0, 0, 0, 220 };

    func_809D0584(thisv, globalCtx);
    if (thisx->colChkInfo.damageEffect != 6) {
        thisv->actionFunc(thisv, globalCtx);
        if (thisv->unk_23C == 0) {
            thisv->unk_23A = (thisv->unk_23A + 4) & 0x7F;
        }
        if ((globalCtx->gameplayFrames & thisv->unk_221) == 0) {
            accel.y = -3.0f;
            velocity.x = Rand_CenteredFloat(thisv->unk_248 * 24.0f);
            velocity.y = thisv->unk_248 * 30.0f;
            velocity.z = Rand_CenteredFloat(thisv->unk_248 * 24.0f);
            accel.x = velocity.x * -0.075f;
            accel.z = velocity.z * -0.075f;
            func_8002836C(globalCtx, &thisx->world.pos, &velocity, &accel, &sp50, &sp4C, 0x3C, 0, 0x14);
        }
        if (thisv->unk_248 <= 0.4f) {
            thisv->collider1.info.toucher.effect = 0;
            if (((globalCtx->gameplayFrames & 1) == 0) && (thisv->unk_220 < 5) && (thisv->unk_23C == 0)) {
                accel.y = -0.1f;
                velocity.x = Rand_CenteredFloat(4.0f);
                velocity.y = Rand_CenteredFloat(2.0f) + 6.0f;
                velocity.z = Rand_CenteredFloat(4.0f);
                accel.x = velocity.x * -0.1f;
                accel.z = velocity.z * -0.1f;
                Rand_CenteredFloat(4.0f);
                Rand_CenteredFloat(4.0f);
                sp48.a = thisv->color1.a;
                if (sp48.a >= 30) {
                    sp44.a = sp48.a - 30;
                } else {
                    sp44.a = 0;
                }
                func_8002836C(globalCtx, &thisx->world.pos, &velocity, &accel, &sp48, &sp44, 0xB4, 0x28,
                              20.0f - (thisv->unk_248 * 40.0f));
            }
        } else {
            thisv->collider1.info.toucher.effect = 1;
        }

        thisv->unk_234 = Actor_TestFloorInDirection(thisx, globalCtx, 50.0f, thisx->world.rot.y);
        if ((thisv->unk_220 == 4) || (thisv->unk_220 == 6) || (thisv->unk_220 == 5) || (thisv->unk_220 == 1) ||
            (thisv->unk_234 != 0)) {
            Actor_MoveForward(thisx);
        }
        Actor_UpdateBgCheckInfo(globalCtx, thisx, 20.0f, 30.0f, 21.0f, 0x1F);
    }
    Collider_UpdateCylinder(thisx, &thisv->collider2);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
    if ((thisv->unk_220 != 0) && ((thisx->colorFilterTimer == 0) || !(thisx->colorFilterParams & 0x4000))) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
    }
    if ((thisv->unk_221 != 1) && (thisv->unk_220 < 5) && (thisv->unk_248 > 0.4f)) {
        Collider_UpdateCylinder(thisx, &thisv->collider1);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
    }
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 5.0f;
}

s32 EnBw_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnBw* thisv = (EnBw*)thisx;

    if (limbIndex == 1) {
        gSPSegment((*gfx)++, 0x09,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, 0, thisv->unk_23A, 0x20, 0x20));
        if ((thisv->unk_220 == 1) || (thisv->unk_220 == 5)) {
            Matrix_Push();
            Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            Matrix_RotateX(thisv->unk_258 * 0.115f, MTXMODE_APPLY);
            Matrix_RotateY(thisv->unk_258 * 0.13f, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->unk_258 * 0.1f, MTXMODE_APPLY);
            Matrix_Scale(1.0f - thisv->unk_260, 1.0f + thisv->unk_260, 1.0f - thisv->unk_260, MTXMODE_APPLY);
            Matrix_RotateZ(-(thisv->unk_258 * 0.1f), MTXMODE_APPLY);
            Matrix_RotateY(-(thisv->unk_258 * 0.13f), MTXMODE_APPLY);
            Matrix_RotateX(-(thisv->unk_258 * 0.115f), MTXMODE_APPLY);
            gSPMatrix((*gfx)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bw.c", 1388),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList((*gfx)++, *dList);
            Matrix_Pop();
            return 1;
        }
    }
    return 0;
}

static Vec3f sIceOffsets[] = {
    { 20.0f, 10.0f, 0.0f },  { -20.0f, 10.0f, 0.0f }, { 0.0f, 10.0f, -25.0f }, { 10.0f, 0.0f, 15.0f },
    { 10.0f, 0.0f, -15.0f }, { 0.0f, 10.0f, 25.0f },  { -10.0f, 0.0f, 15.0f }, { -10.0f, 0.0f, -15.0f },
};

void EnBw_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    Vec3f spAC = { 0.0f, 0.0f, 0.0f };
    GlobalContext* globalCtx = globalCtx2;
    EnBw* thisv = (EnBw*)thisx;
    Vec3f icePos;
    s32 iceIndex;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bw.c", 1423);

    if (thisv->color1.a == 0xFF) {
        func_80093D18(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->color1.r, thisv->color1.g, thisv->color1.b, thisv->color1.a);
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        POLY_OPA_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnBw_OverrideLimbDraw, NULL, thisv, POLY_OPA_DISP);
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 0, 0, thisv->color1.a);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->color1.r, thisv->color1.g, thisv->color1.b, thisv->color1.a);
        gSPSegment(POLY_XLU_DISP++, 0x08, &D_80116280[0]);
        POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnBw_OverrideLimbDraw, NULL, thisv, POLY_XLU_DISP);
    }

    if (((globalCtx->gameplayFrames + 1) % 4) == (u32)thisx->params) {
        spAC.z = thisx->scale.z * 375000.0f;
        Matrix_MultVec3f(&spAC, &thisv->unk_264);
        spAC.z = thisx->scale.z * 150000.0f;
        Matrix_MultVec3f(&spAC, &thisv->unk_270);
        spAC.x = thisx->scale.x * 550000.0f;
        Matrix_MultVec3f(&spAC, &thisv->unk_288);
        spAC.x = -spAC.x;
        Matrix_MultVec3f(&spAC, &thisv->unk_27C);
    }

    Matrix_Translate(thisx->world.pos.x, thisx->world.pos.y + ((thisx->scale.y - 0.013f) * 1000.0f), thisx->world.pos.z,
                     MTXMODE_NEW);
    func_80093D84(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                (globalCtx->gameplayFrames * -20) % 0x200, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
    Matrix_Scale(thisv->unk_248 * 0.01f, thisv->unk_248 * 0.01f, thisv->unk_248 * 0.01f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bw.c", 1500),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    if (thisv->iceTimer != 0) {
        thisx->colorFilterTimer++;
        thisv->iceTimer--;
        if ((thisv->iceTimer & 3) == 0) {
            iceIndex = thisv->iceTimer >> 2;

            icePos.x = sIceOffsets[iceIndex].x + thisx->world.pos.x;
            icePos.y = sIceOffsets[iceIndex].y + thisx->world.pos.y;
            icePos.z = sIceOffsets[iceIndex].z + thisx->world.pos.z;
            EffectSsEnIce_SpawnFlyingVec3f(globalCtx, thisx, &icePos, 0x96, 0x96, 0x96, 0xFA, 0xEB, 0xF5, 0xFF, 1.3f);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bw.c", 1521);
}

void EnBw_Reset(void) {
    sSlugGroup = 0;
}