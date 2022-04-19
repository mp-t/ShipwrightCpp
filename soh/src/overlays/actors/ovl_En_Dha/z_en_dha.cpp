/*
 * File: z_en_dha.c
 * Overlay: ovl_En_Dha
 * Description: Dead Hand's Hand
 */

#include "z_en_dha.h"
#include "overlays/actors/ovl_En_Dh/z_en_dh.h"
#include "objects/object_dh/object_dh.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnDha_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDha_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDha_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDha_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDha_SetupWait(EnDha* thisv);
void EnDha_Wait(EnDha* thisv, GlobalContext* globalCtx);
void EnDha_SetupTakeDamage(EnDha* thisv);
void EnDha_TakeDamage(EnDha* thisv, GlobalContext* globalCtx);
void EnDha_SetupDeath(EnDha* thisv);
void EnDha_Die(EnDha* thisv, GlobalContext* globalCtx);
void EnDha_UpdateHealth(EnDha* thisv, GlobalContext* globalCtx);

ActorInit En_Dha_InitVars = {
    ACTOR_EN_DHA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DH,
    sizeof(EnDha),
    (ActorFunc)EnDha_Init,
    (ActorFunc)EnDha_Destroy,
    (ActorFunc)EnDha_Update,
    (ActorFunc)EnDha_Draw,
    NULL,
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(2, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(2, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(4, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 1, { { 0, 0, 0 }, 12 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 2, { { 3200, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 3, { { 1200, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 4, { { 2700, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 5, { { 1200, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    5,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x2E, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void EnDha_SetupAction(EnDha* thisv, EnDhaActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnDha_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDha* thisv = (EnDha*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_dh_Skel_000BD8, &object_dh_Anim_0015B0, thisv->jointTable,
                       thisv->morphTable, 4);
    ActorShape_Init(&thisv->actor.shape, 0, ActorShadow_DrawFeet, 90.0f);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 50.0f;
    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisv->actor.colChkInfo.health = 8;
    thisv->limbAngleX[0] = -0x4000;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItem);
    thisv->actor.flags &= ~ACTOR_FLAG_0;

    EnDha_SetupWait(thisv);
}

void EnDha_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDha* thisv = (EnDha*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnDha_SetupWait(EnDha* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_dh_Anim_0015B0);
    thisv->unk_1C0 = 0;
    thisv->actionTimer = ((Rand_ZeroOne() * 10.0f) + 5.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actor.home.rot.z = 1;
    EnDha_SetupAction(thisv, EnDha_Wait);
}

void EnDha_Wait(EnDha* thisv, GlobalContext* globalCtx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f armPosMultiplier1 = { 0.0f, 0.0f, 55.0f };
    Vec3f armPosMultiplier2 = { 0.0f, 0.0f, -54.0f };
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    s32 pad2;
    Vec3f playerPos = player->actor.world.pos;
    Vec3s angle;
    s16 yaw;

    playerPos.x += Math_SinS(player->actor.shape.rot.y) * -5.0f;
    playerPos.z += Math_CosS(player->actor.shape.rot.y) * -5.0f;

    if (!LINK_IS_ADULT) {
        playerPos.y += 38.0f;
    } else {
        playerPos.y += 56.0f;
    }

    if (thisv->actor.xzDistToPlayer <= 100.0f) {
        thisv->handAngle.y = thisv->handAngle.x = thisv->limbAngleY = 0;

        if (Math_Vec3f_DistXYZ(&playerPos, &thisv->handPos[0]) <= 12.0f) {
            if (thisv->unk_1CC == 0) {
                if (globalCtx->grabPlayer(globalCtx, player)) {
                    thisv->timer = 0;
                    thisv->unk_1CC++;

                    if (thisv->actor.parent != NULL) {
                        thisv->actor.parent->params = ENDH_START_ATTACK_GRAB;
                    }

                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_GRIP);
                }
            } else {
                thisv->timer += 0x1194;
                thisv->limbAngleY = Math_SinS(thisv->timer) * 1820.0f;

                if (!(player->stateFlags2 & 0x80)) {
                    thisv->unk_1CC = 0;
                    EnDha_SetupTakeDamage(thisv);
                    return;
                }

                if (thisv->timer < -0x6E6B) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_GRIP);
                }
            }

            func_80035844(&thisv->handPos[1], &playerPos, &thisv->handAngle, 0);
            thisv->handAngle.y -= thisv->actor.shape.rot.y + thisv->limbAngleY;
            thisv->handAngle.x -= thisv->actor.shape.rot.x + thisv->limbAngleX[0] + thisv->limbAngleX[1];
        } else {
            if ((player->stateFlags2 & 0x80) && (&thisv->actor == player->actor.parent)) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
            }

            if (thisv->actor.home.rot.z != 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_HAND_AT);
                thisv->actor.home.rot.z = 0;
            }
        }

        thisv->actor.shape.rot.y = Math_Vec3f_Yaw(&thisv->actor.world.pos, &playerPos);

        Math_SmoothStepToF(&thisv->handPos[0].x, playerPos.x, 1.0f, 16.0f, 0.0f);
        Math_SmoothStepToF(&thisv->handPos[0].y, playerPos.y, 1.0f, 16.0f, 0.0f);
        Math_SmoothStepToF(&thisv->handPos[0].z, playerPos.z, 1.0f, 16.0f, 0.0f);

        func_80035844(&thisv->armPos, &thisv->handPos[0], &angle, 0);
        Matrix_Translate(thisv->handPos[0].x, thisv->handPos[0].y, thisv->handPos[0].z, MTXMODE_NEW);
        Matrix_RotateZYX(angle.x, angle.y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&armPosMultiplier2, &thisv->armPos);
        Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
        func_80035844(&thisv->actor.world.pos, &thisv->armPos, &angle, 0);
        Matrix_RotateZYX(angle.x, angle.y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&armPosMultiplier1, &thisv->armPos);
        thisv->limbAngleX[0] = Math_Vec3f_Pitch(&thisv->actor.world.pos, &thisv->armPos);
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->armPos) - thisv->actor.shape.rot.y;

        if (ABS(yaw) >= 0x4000) {
            thisv->limbAngleX[0] = -0x8000 - thisv->limbAngleX[0];
        }

        thisv->limbAngleX[1] = (Math_Vec3f_Pitch(&thisv->armPos, &thisv->handPos[0]) - thisv->limbAngleX[0]);

        if (thisv->limbAngleX[1] < 0) {
            thisv->limbAngleX[0] += thisv->limbAngleX[1] * 2;
            thisv->limbAngleX[1] *= -2;
        }
    } else {
        if ((player->stateFlags2 & 0x80) && (&thisv->actor == player->actor.parent)) {
            player->stateFlags2 &= ~0x80;
            player->actor.parent = NULL;
            player->unk_850 = 200;
        }

        thisv->actor.home.rot.z = 1;
        Math_SmoothStepToS(&thisv->limbAngleX[1], 0, 1, 0x3E8, 0);
        Math_SmoothStepToS(&thisv->limbAngleX[0], -0x4000, 1, 0x3E8, 0);
        SkelAnime_Update(&thisv->skelAnime);
    }
}

void EnDha_SetupTakeDamage(EnDha* thisv) {
    thisv->actionTimer = 15;
    EnDha_SetupAction(thisv, EnDha_TakeDamage);
}

void EnDha_TakeDamage(EnDha* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((player->stateFlags2 & 0x80) && (&thisv->actor == player->actor.parent)) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
    }

    Math_SmoothStepToS(&thisv->limbAngleX[1], 0, 1, 2000, 0);
    Math_SmoothStepToS(&thisv->limbAngleY, 0, 1, 600, 0);
    Math_SmoothStepToS(&thisv->limbAngleX[0], -0x4000, 1, 2000, 0);
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        EnDha_SetupWait(thisv);
    }
}

void EnDha_SetupDeath(EnDha* thisv) {
    thisv->unk_1C0 = 8;
    thisv->actionTimer = 300;

    if (thisv->actor.parent != NULL) {
        if (thisv->actor.parent->params != ENDH_DEATH) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_HAND_DEAD);
        }
        if (thisv->actor.parent->params <= ENDH_WAIT_UNDERGROUND) {
            thisv->actor.parent->params--;
        }
    }

    EnDha_SetupAction(thisv, EnDha_Die);
}

void EnDha_Die(EnDha* thisv, GlobalContext* globalCtx) {
    s16 angle;
    Vec3f vec;
    Player* player = GET_PLAYER(globalCtx);

    if ((player->stateFlags2 & 0x80) && (&thisv->actor == player->actor.parent)) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
    }

    Math_SmoothStepToS(&thisv->limbAngleX[1], 0, 1, 0x7D0, 0);
    angle = Math_SmoothStepToS(&thisv->limbAngleX[0], -0x4000, 1, 0x7D0, 0);
    SkelAnime_Update(&thisv->skelAnime);

    if (angle == 0) {
        vec = thisv->actor.world.pos;

        if (thisv->actionTimer != 0) {
            if (-12000.0f < thisv->actor.shape.yOffset) {
                thisv->actor.shape.yOffset -= 1000.0f;
                func_80033480(globalCtx, &vec, 7.0f, 1, 0x5A, 0x14, 1);
            } else {
                thisv->actionTimer--;

                if ((thisv->actor.parent != NULL) && (thisv->actor.parent->params == ENDH_DEATH)) {
                    Actor_Kill(&thisv->actor);
                }
            }
        } else {
            thisv->actor.shape.yOffset += 500.0f;
            func_80033480(globalCtx, &vec, 7.0f, 1, 0x5A, 0x14, 1);

            if (thisv->actor.shape.yOffset == 0.0f) {
                EnDha_SetupWait(thisv);
            }
        }
    }
}

void EnDha_UpdateHealth(EnDha* thisv, GlobalContext* globalCtx) {
    if (!((thisv->unk_1C0 >= 8) || !(thisv->collider.base.acFlags & AC_HIT))) {
        thisv->collider.base.acFlags &= ~AC_HIT;

        if (thisv->actor.colChkInfo.damageEffect == 0 || thisv->actor.colChkInfo.damageEffect == 6) {
            return;
        } else {
            Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                EnDha_SetupDeath(thisv);
                thisv->actor.colChkInfo.health = 8;
                Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xE0);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_DAMAGE);
                thisv->unk_1C0 = 9;
                EnDha_SetupTakeDamage(thisv);
            }
        }
    }

    if ((thisv->actor.parent != NULL) && (thisv->actor.parent->params == ENDH_DEATH)) {
        EnDha_SetupDeath(thisv);
    }
}

void EnDha_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDha* thisv = (EnDha*)thisx;

    if (thisv->actor.parent == NULL) {
        thisv->actor.parent = Actor_FindNearby(globalCtx, &thisv->actor, ACTOR_EN_DH, ACTORCAT_ENEMY, 10000.0f);
    }

    EnDha_UpdateHealth(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

s32 EnDha_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDha* thisv = (EnDha*)thisx;

    if (limbIndex == 1) {
        rot->y = -(s16)(thisv->limbAngleX[0] + 0x4000);
        rot->z += thisv->limbAngleY;
    } else if (limbIndex == 2) {
        rot->z = thisv->limbAngleX[1];
        rot->y -= thisv->limbAngleY;
    } else if (limbIndex == 3) {
        rot->y = -thisv->handAngle.y;
        rot->z = -thisv->handAngle.x;
    }

    return false;
}

void EnDha_OverridePostDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f handVec = { 1100.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    EnDha* thisv = (EnDha*)thisx;

    switch (limbIndex) {
        case 1:
            Collider_UpdateSpheres(2, &thisv->collider);
            Collider_UpdateSpheres(3, &thisv->collider);
            break;
        case 2:
            Collider_UpdateSpheres(4, &thisv->collider);
            Collider_UpdateSpheres(5, &thisv->collider);
            Matrix_MultVec3f(&zeroVec, &thisv->armPos);
            break;
        case 3:
            Collider_UpdateSpheres(1, &thisv->collider);
            Matrix_MultVec3f(&handVec, &thisv->handPos[0]);
            Matrix_MultVec3f(&zeroVec, &thisv->handPos[1]);
            break;
    }
}

void EnDha_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDha* thisv = (EnDha*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnDha_OverrideLimbDraw, EnDha_OverridePostDraw, thisv);
}
