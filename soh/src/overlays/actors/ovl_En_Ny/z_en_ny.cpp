#include "z_en_ny.h"
#include "objects/object_ny/object_ny.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnNy_Init(Actor* thisx, GlobalContext* globalCtx);
void EnNy_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnNy_Update(Actor* thisx, GlobalContext* globalCtx);
void EnNy_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnNy_UpdateUnused(Actor* thisx, GlobalContext* globalCtx);
void EnNy_Move(EnNy* thisv, GlobalContext* globalCtx);
void EnNy_Die(EnNy* thisv, GlobalContext* globalCtx);
void func_80ABCD40(EnNy* thisv);
void func_80ABCDBC(EnNy* thisv);
void EnNy_TurnToStone(EnNy* thisv, GlobalContext* globalCtx);
void func_80ABD11C(EnNy* thisv, GlobalContext* globalCtx);
void func_80ABCE50(EnNy* thisv, GlobalContext* globalCtx);
void func_80ABCE90(EnNy* thisv, GlobalContext* globalCtx);
void func_80ABCEEC(EnNy* thisv, GlobalContext* globalCtx);
void EnNy_UpdateDeath(Actor* thisx, GlobalContext* GlobalContext);
void EnNy_SetupDie(EnNy* thisv, GlobalContext* globalCtx);
void EnNy_DrawDeathEffect(Actor* thisx, GlobalContext* GlobalContext);
void func_80ABD3B8(EnNy* thisv, f32, f32);

const ActorInit En_Ny_InitVars = {
    ACTOR_EN_NY,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_NY,
    sizeof(EnNy),
    (ActorFunc)EnNy_Init,
    (ActorFunc)EnNy_Destroy,
    (ActorFunc)EnNy_Update,
    (ActorFunc)EnNy_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x04, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sColliderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x28, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnNy_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnNy* thisv = (EnNy*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    thisv->actor.colChkInfo.health = 2;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sColliderInit, thisv->elements);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    thisv->unk_1CA = 0;
    thisv->unk_1D0 = 0;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.shape.rot.y = 0;
    thisv->actor.gravity = -0.4f;
    thisv->hitPlayer = 0;
    thisv->unk_1CE = 2;
    thisv->actor.velocity.y = 0.0f;
    thisv->unk_1D4 = 0xFF;
    thisv->unk_1D8 = 0;
    thisv->unk_1E8 = 0.0f;
    thisv->unk_1E0 = 0.25f;
    if (thisv->actor.params == 0) {
        // "New initials"
        osSyncPrintf("ニュウ イニシャル[ %d ] ！！\n", thisv->actor.params);
        thisv->actor.colChkInfo.mass = 0;
        thisv->unk_1D4 = 0;
        thisv->unk_1D8 = 0xFF;
        thisv->unk_1E0 = 1.0f;
        func_80ABCDBC(thisv);
    } else {
        // This mode is unused in the final game
        // "Dummy new initials"
        osSyncPrintf("ダミーニュウ イニシャル[ %d ] ！！\n", thisv->actor.params);
        osSyncPrintf("En_Ny_actor_move2[ %x ] ！！\n", EnNy_UpdateUnused);
        thisv->actor.colChkInfo.mass = 0xFF;
        thisv->collider.base.colType = COLTYPE_METAL;
        thisv->actor.update = EnNy_UpdateUnused;
    }
}

void EnNy_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnNy* thisv = (EnNy*)thisx;
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void func_80ABCD40(EnNy* thisv) {
    f32 temp;

    temp = (thisv->actor.yDistToWater > 0.0f) ? 0.7f : 1.0f;
    thisv->unk_1E8 = 2.8f * temp;
}

void func_80ABCD84(EnNy* thisv) {
    thisv->actionFunc = func_80ABCE50;
}

void func_80ABCD94(EnNy* thisv) {
    thisv->stoneTimer = 0x14;
    thisv->actionFunc = func_80ABCE90;
}

void func_80ABCDAC(EnNy* thisv) {
    thisv->actionFunc = func_80ABCEEC;
}

void func_80ABCDBC(EnNy* thisv) {
    thisv->unk_1F4 = 0.0f;
    func_80ABCD40(thisv);
    thisv->stoneTimer = 180;
    thisv->actionFunc = EnNy_Move;
}

void EnNy_SetupTurnToStone(EnNy* thisv) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NYU_HIT_STOP);
    thisv->actionFunc = EnNy_TurnToStone;
    thisv->unk_1E8 = 0.0f;
}

void func_80ABCE38(EnNy* thisv) {
    thisv->stoneTimer = 0x3C;
    thisv->actionFunc = func_80ABD11C;
}

void func_80ABCE50(EnNy* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.xyzDistToPlayerSq <= 25600.0f) {
        func_80ABCD94(thisv);
    }
}

void func_80ABCE90(EnNy* thisv, GlobalContext* globalCtx) {
    s32 phi_v1;
    s32 phi_v0;

    phi_v1 = thisv->unk_1D4 - 0x40;
    phi_v0 = thisv->unk_1D8 + 0x40;
    if (phi_v0 >= 0xFF) {
        phi_v1 = 0;
        phi_v0 = 0xFF;
        func_80ABCDAC(thisv);
    }
    thisv->unk_1D4 = phi_v1;
    thisv->unk_1D8 = phi_v0;
}

void func_80ABCEEC(EnNy* thisv, GlobalContext* globalCtx) {
    f32 phi_f0;

    phi_f0 = thisv->unk_1E0;
    phi_f0 += 2.0f;
    if (phi_f0 >= 1.0f) {
        phi_f0 = 1.0f;
        func_80ABCDBC(thisv);
    }
    thisv->unk_1E0 = phi_f0;
}

void EnNy_Move(EnNy* thisv, GlobalContext* globalCtx) {
    f32 yawDiff;
    s32 stoneTimer;

    if (!(thisv->unk_1F0 < thisv->actor.yDistToWater)) {
        func_8002F974(&thisv->actor, NA_SE_EN_NYU_MOVE - SFX_FLAG);
    }
    func_80ABCD40(thisv);
    stoneTimer = thisv->stoneTimer;
    thisv->stoneTimer--;
    if ((stoneTimer <= 0) || (thisv->hitPlayer != false)) {
        EnNy_SetupTurnToStone(thisv);
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xA, thisv->unk_1F4, 0);
        Math_ApproachF(&thisv->unk_1F4, 2000.0f, 1.0f, 100.0f);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        yawDiff = Math_FAtan2F(thisv->actor.yDistToPlayer, thisv->actor.xzDistToPlayer);
        thisv->actor.speedXZ = fabsf(cosf(yawDiff) * thisv->unk_1E8);
        if (thisv->unk_1F0 < thisv->actor.yDistToWater) {
            thisv->unk_1EC = sinf(yawDiff) * thisv->unk_1E8;
        }
    }
}

void EnNy_TurnToStone(EnNy* thisv, GlobalContext* globalCtx) {
    f32 phi_f0;

    phi_f0 = thisv->unk_1E0;
    phi_f0 -= 2.0f;
    if (phi_f0 <= 0.25f) {
        phi_f0 = 0.25f;
        if (thisv->actor.bgCheckFlags & 2) {
            if (!(thisv->unk_1F0 < thisv->actor.yDistToWater)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
            }
            thisv->actor.bgCheckFlags &= ~2;
            thisv->actor.speedXZ = 0.0f;
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
            func_80ABCE38(thisv);
        }
    }
    thisv->unk_1E0 = phi_f0;
}

void func_80ABD11C(EnNy* thisv, GlobalContext* globalCtx) {
    s32 phi_v0;
    s32 phi_v1;

    phi_v0 = thisv->unk_1D4;
    phi_v0 += 0x40;
    phi_v1 = thisv->unk_1D8;
    phi_v1 -= 0x40;
    if (phi_v0 >= 0xFF) {
        phi_v0 = 0xFF;
        phi_v1 = 0;
        if (thisv->stoneTimer != 0) {
            thisv->stoneTimer--;
        } else {
            func_80ABCD84(thisv);
        }
    }
    thisv->unk_1D4 = phi_v0;
    thisv->unk_1D8 = phi_v1;
}

s32 EnNy_CollisionCheck(EnNy* thisv, GlobalContext* globalCtx) {
    u8 sp3F;
    Vec3f effectPos;

    sp3F = 0;
    thisv->hitPlayer = 0;
    if (thisv->collider.base.atFlags & 4) {
        thisv->collider.base.atFlags &= ~4;
        thisv->hitPlayer = 1;
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.speedXZ = -4.0f;
        return 0;
    }
    if (thisv->collider.base.atFlags & 2) {
        thisv->collider.base.atFlags &= ~2;
        thisv->hitPlayer = 1;
        return 0;
    } else {
        if (thisv->collider.base.acFlags & 2) {
            thisv->collider.base.acFlags &= ~2;
            effectPos.x = thisv->collider.elements[0].info.bumper.hitPos.x;
            effectPos.y = thisv->collider.elements[0].info.bumper.hitPos.y;
            effectPos.z = thisv->collider.elements[0].info.bumper.hitPos.z;
            if ((thisv->unk_1E0 == 0.25f) && (thisv->unk_1D4 == 0xFF)) {
                switch (thisv->actor.colChkInfo.damageEffect) {
                    case 0xE:
                        sp3F = 1;
                    case 0xF:
                        Actor_ApplyDamage(&thisv->actor);
                        Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 0x50);
                        break;
                    case 1:
                        Actor_ApplyDamage(&thisv->actor);
                        Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 0x50);
                        break;
                    case 2:
                        thisv->unk_1CA = 4;
                        Actor_ApplyDamage(&thisv->actor);
                        Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 0x50);
                        break;
                }
            }
            thisv->stoneTimer = 0;
            if (thisv->actor.colChkInfo.health == 0) {
                thisv->actor.shape.shadowAlpha = 0;
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                thisv->unk_1D0 = sp3F;
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                return 1;
            }
            EffectSsHitMark_SpawnFixedScale(globalCtx, 0, &effectPos);
            return 0;
        }
    }
    return 0;
}

void func_80ABD3B8(EnNy* thisv, f32 arg1, f32 arg2) {
    if (thisv->unk_1E8 == 0.0f) {
        thisv->actor.gravity = -0.4f;
    } else if (!(arg1 < thisv->actor.yDistToWater)) {
        thisv->actor.gravity = -0.4f;
    } else if (arg2 < thisv->actor.yDistToWater) {
        thisv->actor.gravity = 0.0;
        if (thisv->unk_1EC < thisv->actor.velocity.y) {
            thisv->actor.velocity.y -= 0.4f;
            if (thisv->actor.velocity.y < thisv->unk_1EC) {
                thisv->actor.velocity.y = thisv->unk_1EC;
            }
        } else if (thisv->actor.velocity.y < thisv->unk_1EC) {
            thisv->actor.velocity.y += 0.4f;
            if (thisv->unk_1EC < thisv->actor.velocity.y) {
                thisv->actor.velocity.y = thisv->unk_1EC;
            }
        }
    }
}

void EnNy_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnNy* thisv = (EnNy*)thisx;
    f32 temp_f20;
    f32 temp_f22;
    s32 i;

    thisv->timer++;
    temp_f20 = thisv->unk_1E0 - 0.25f;
    if (thisv->unk_1CA != 0) {
        thisv->unk_1CA--;
    }
    Actor_SetFocus(&thisv->actor, 0.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->collider.elements[0].dim.scale = 1.33f * temp_f20 + 1.0f;
    temp_f22 = (24.0f * temp_f20) + 12.0f;
    thisv->actor.shape.rot.x += (s16)(thisv->unk_1E8 * 1000.0f);
    func_80ABD3B8(thisv, temp_f22 + 10.0f, temp_f22 - 10.0f);
    Actor_MoveForward(&thisv->actor);
    Math_StepToF(&thisv->unk_1E4, thisv->unk_1E8, 0.1f);
    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.prevPos.y -= temp_f22;
    thisv->actor.world.pos.y -= temp_f22;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 60.0f, 7);
    thisv->unk_1F0 = temp_f22;
    thisv->actor.world.pos.y += temp_f22;
    if (EnNy_CollisionCheck(thisv, globalCtx) != 0) {
        for (i = 0; i < 8; i++) {
            thisv->unk_1F8[i].x = (Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.x);
            thisv->unk_1F8[i].y = (Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.y);
            thisv->unk_1F8[i].z = (Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.z);
        }
        thisv->timer = 0;
        thisv->actor.update = EnNy_UpdateDeath;
        thisv->actor.draw = EnNy_DrawDeathEffect;
        thisv->actionFunc = EnNy_SetupDie;
        return;
    }
    if (thisv->unk_1E0 > 0.25f) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnNy_SetupDie(EnNy* thisv, GlobalContext* globalCtx) {
    s32 effectScale;
    s32 i;
    Vec3f effectPos;
    Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f effectAccel = { 0.0f, 0.1f, 0.0f };

    if (thisv->timer >= 2) {
        if (thisv->actor.yDistToWater > 0.0f) {
            for (i = 0; i < 10; i++) {
                effectPos.x = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.x;
                effectPos.y = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.y;
                effectPos.z = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.z;
                effectScale = Rand_S16Offset(0x50, 0x64);
                EffectSsDtBubble_SpawnColorProfile(globalCtx, &effectPos, &effectVelocity, &effectAccel, effectScale,
                                                   25, 0, 1);
            }
            for (i = 0; i < 0x14; i++) {
                effectPos.x = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.x;
                effectPos.y = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.y;
                effectPos.z = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.z;
                EffectSsBubble_Spawn(globalCtx, &effectPos, 10.0f, 10.0f, 30.0f, 0.25f);
            }
        }
        for (i = 0; i < 8; i++) {
            thisv->unk_1F8[i + 8].x = Rand_CenteredFloat(10.0f);
            thisv->unk_1F8[i + 8].z = Rand_CenteredFloat(10.0f);
            thisv->unk_1F8[i + 8].y = Rand_ZeroFloat(4.0f) + 4.0f;
        }
        thisv->timer = 0;
        if (thisv->unk_1D0 == 0) {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xA0);
        } else {
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, 8);
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NYU_DEAD);
        thisv->actionFunc = EnNy_Die;
    }
}

void EnNy_Die(EnNy* thisv, GlobalContext* globalCtx) {
    s32 i;

    if (thisv->actor.yDistToWater > 0.0f) {
        for (i = 0; i < 8; i += 1) {
            thisv->unk_1F8[i].x += thisv->unk_1F8[i + 8].x;
            thisv->unk_1F8[i].y += thisv->unk_1F8[i + 8].y;
            thisv->unk_1F8[i].z += thisv->unk_1F8[i + 8].z;
            Math_StepToF(&thisv->unk_1F8[i + 8].x, 0.0f, 0.1f);
            Math_StepToF(&thisv->unk_1F8[i + 8].y, -1.0f, 0.4f);
            Math_StepToF(&thisv->unk_1F8[i + 8].z, 0.0f, 0.1f);
        }
        if (thisv->timer >= 0x1F) {
            Actor_Kill(&thisv->actor);
            return;
        }
    } else {
        for (i = 0; i < 8; i += 1) {
            thisv->unk_1F8[i].x += thisv->unk_1F8[i + 8].x;
            thisv->unk_1F8[i].y += thisv->unk_1F8[i + 8].y;
            thisv->unk_1F8[i].z += thisv->unk_1F8[i + 8].z;
            Math_StepToF(&thisv->unk_1F8[i + 8].x, 0.0f, 0.15f);
            Math_StepToF(&thisv->unk_1F8[i + 8].y, -1.0f, 0.6f);
            Math_StepToF(&thisv->unk_1F8[i + 8].z, 0.0f, 0.15f);
        }
        if (thisv->timer >= 0x10) {
            Actor_Kill(&thisv->actor);
            return;
        }
    }
}

void EnNy_UpdateDeath(Actor* thisx, GlobalContext* globalCtx) {
    EnNy* thisv = (EnNy*)thisx;

    thisv->timer++;
    if (thisv->unk_1CA != 0) {
        thisv->unk_1CA--;
    }
    thisv->actionFunc(thisv, globalCtx);
}

void EnNy_UpdateUnused(Actor* thisx, GlobalContext* globalCtx2) {
    EnNy* thisv = (EnNy*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    f32 sp3C;
    f32 temp_f0;

    sp3C = thisv->unk_1E0 - 0.25f;
    thisv->timer++;
    Actor_SetFocus(&thisv->actor, 0.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    temp_f0 = (24.0f * sp3C) + 12.0f;
    thisv->actor.prevPos.y -= temp_f0;
    thisv->actor.world.pos.y -= temp_f0;

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 60.0f, 7);
    thisv->unk_1F0 = temp_f0;
    thisv->actor.world.pos.y += temp_f0;

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Math_StepToF(&thisv->unk_1E4, thisv->unk_1E8, 0.1f);
}
static Vec3f sFireOffsets[] = {
    { 5.0f, 0.0f, 0.0f },
    { -5.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 5.0f },
    { 0.0f, 0.0f, -5.0f },
};

void EnNy_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnNy* thisv = (EnNy*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ny.c", 837);
    Collider_UpdateSpheres(0, &thisv->collider);
    func_8002ED80(&thisv->actor, globalCtx, 1);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ny.c", 845),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->unk_1D8);
    gSPDisplayList(POLY_XLU_DISP++, gEnNyMetalBodyDL);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->unk_1D4);
    gSPDisplayList(POLY_XLU_DISP++, gEnNyRockBodyDL);
    if (thisv->unk_1E0 > 0.25f) {
        Matrix_Scale(thisv->unk_1E0, thisv->unk_1E0, thisv->unk_1E0, MTXMODE_APPLY);
        func_8002EBCC(&thisv->actor, globalCtx, 1);
        func_80093D18(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ny.c", 868),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gEnNySpikeDL);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ny.c", 872);
    if (thisv->unk_1CA != 0) {
        Vec3f tempVec;
        Vec3f* fireOffset;
        s16 temp;

        temp = thisv->unk_1CA - 1;
        thisv->actor.colorFilterTimer++;
        if (temp == 0) {
            fireOffset = &sFireOffsets[temp & 3];
            tempVec.x = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.x + fireOffset->x);
            tempVec.y = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.y + fireOffset->y);
            tempVec.z = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.z + fireOffset->z);
            EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &tempVec, 100, 0, 0, -1);
        }
    }
}

void EnNy_DrawDeathEffect(Actor* thisx, GlobalContext* globalCtx) {
    EnNy* thisv = (EnNy*)thisx;
    Vec3f* temp;
    f32 scale;
    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ny.c", 900);
    func_80093D18(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0x00, 0x00, 0x00, 0xFF);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gDPPipeSync(POLY_OPA_DISP++);
    for (i = 0; i < 8; i++) {
        if (thisv->timer < (i + 22)) {
            temp = &thisv->unk_1F8[i];
            Matrix_Translate(temp->x, temp->y, temp->z, MTXMODE_NEW);
            scale = thisv->actor.scale.x * 0.4f * (1.0f + (i * 0.04f));
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ny.c", 912),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gEnNyRockBodyDL);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ny.c", 919);
    if (thisv->unk_1CA != 0) {
        Vec3f tempVec;
        Vec3f* fireOffset;
        s16 fireOffsetIndex;

        fireOffsetIndex = thisv->unk_1CA - 1;
        thisv->actor.colorFilterTimer++;
        if ((fireOffsetIndex & 1) == 0) {
            fireOffset = &sFireOffsets[fireOffsetIndex & 3];
            tempVec.x = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.x + fireOffset->x);
            tempVec.y = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.y + fireOffset->y);
            tempVec.z = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.z + fireOffset->z);
            EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &tempVec, 100, 0, 0, -1);
        }
    }
}
