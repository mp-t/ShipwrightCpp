/*
 * File: z_en_ik.c
 * Overlay: ovl_En_Ik
 * Description: Iron Knuckle
 */

#include "z_en_ik.h"
#include "scenes/dungeons/jyasinboss/jyasinboss_scene.h"
#include "objects/object_ik/object_ik.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

typedef void (*EnIkDrawFunc)(struct EnIk*, GlobalContext*);

void EnIk_Init(Actor* thisx, GlobalContext* globalCtx);
void EnIk_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnIk_Update(Actor* thisx, GlobalContext* globalCtx);
void EnIk_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A74714(EnIk* thisv);
void func_80A747C0(EnIk* thisv, GlobalContext* globalCtx);
void func_80A7492C(EnIk* thisv, GlobalContext* globalCtx);
void func_80A74AAC(EnIk* thisv);
void func_80A74BA4(EnIk* thisv, GlobalContext* globalCtx);
void func_80A74E2C(EnIk* thisv);
void func_80A74EBC(EnIk* thisv, GlobalContext* globalCtx);
void func_80A7506C(EnIk* thisv);
void func_80A7510C(EnIk* thisv, GlobalContext* globalCtx);
void func_80A751C8(EnIk* thisv);
void func_80A75260(EnIk* thisv, GlobalContext* globalCtx);
void func_80A753D0(EnIk* thisv);
void func_80A7545C(EnIk* thisv, GlobalContext* globalCtx);
void func_80A754A0(EnIk* thisv);
void func_80A75530(EnIk* thisv, GlobalContext* globalCtx);
void func_80A755F0(EnIk* thisv);
void func_80A7567C(EnIk* thisv, GlobalContext* globalCtx);
void func_80A758B0(EnIk* thisv, GlobalContext* globalCtx);
void func_80A75A38(EnIk* thisv, GlobalContext* globalCtx);
void func_80A75FA0(Actor* thisx, GlobalContext* globalCtx);
void func_80A76798(Actor* thisx, GlobalContext* globalCtx);
void func_80A7748C(EnIk* thisv, GlobalContext* globalCtx);
void func_80A774BC(EnIk* thisv, GlobalContext* globalCtx);
void func_80A774F8(EnIk* thisv, GlobalContext* globalCtx);
void func_80A77844(EnIk* thisv, GlobalContext* globalCtx);
void func_80A779DC(EnIk* thisv, GlobalContext* globalCtx);
void func_80A77AEC(EnIk* thisv, GlobalContext* globalCtx);
void func_80A77B0C(EnIk* thisv, GlobalContext* globalCtx);
void func_80A77B3C(EnIk* thisv, GlobalContext* globalCtx);
void func_80A77ED0(EnIk* thisv, GlobalContext* globalCtx);
void func_80A77EDC(EnIk* thisv, GlobalContext* globalCtx);
void func_80A78160(EnIk* thisv, GlobalContext* globalCtx);
void func_80A781CC(Actor* thisx, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 25, 80, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC3FFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC3FFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
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
        { 0x20000000, 0x00, 0x40 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0xD),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(1, 0xE),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0xD),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(0, 0xD),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(2, 0xE),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(15, 0xE),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(10, 0xF),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

void EnIk_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;

    if (Actor_FindNearby(globalCtx, &thisv->actor, ACTOR_EN_IK, ACTORCAT_ENEMY, 8000.0f) == NULL) {
        func_800F5B58();
    }

    Collider_DestroyTris(globalCtx, &thisv->shieldCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
    Collider_DestroyQuad(globalCtx, &thisv->axeCollider);
}

void EnIk_SetupAction(EnIk* thisv, EnIkActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void func_80A74398(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;
    s32 pad;
    EffectBlureInit1 blureInit;

    thisx->update = func_80A75FA0;
    thisx->draw = func_80A76798;
    thisx->flags |= ACTOR_FLAG_10;

    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinder(globalCtx, &thisv->bodyCollider, thisx, &sCylinderInit);
    Collider_InitTris(globalCtx, &thisv->shieldCollider);
    Collider_SetTris(globalCtx, &thisv->shieldCollider, thisx, &sTrisInit, thisv->shieldColliderItems);
    Collider_InitQuad(globalCtx, &thisv->axeCollider);
    Collider_SetQuad(globalCtx, &thisv->axeCollider, thisx, &sQuadInit);

    thisx->colChkInfo.damageTable = &sDamageTable;
    thisx->colChkInfo.mass = MASS_HEAVY;
    thisv->unk_2FC = 0;
    thisx->colChkInfo.health = 30;
    thisx->gravity = -1.0f;
    thisv->switchFlags = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;

    if (thisx->params == 0) {
        thisx->colChkInfo.health += 20;
        thisx->naviEnemyId = 0x34;
    } else {
        Actor_SetScale(thisx, 0.012f);
        thisx->naviEnemyId = 0x35;
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisx, ACTORCAT_ENEMY);
    }

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p2StartColor[0] = blureInit.p2StartColor[1] =
        blureInit.p2StartColor[2] = blureInit.p1EndColor[0] = blureInit.p1EndColor[1] = blureInit.p2EndColor[0] =
            blureInit.p2EndColor[1] = blureInit.p2EndColor[2] = 255;

    blureInit.p2StartColor[3] = 64;
    blureInit.p1StartColor[3] = 200;
    blureInit.p1StartColor[2] = blureInit.p1EndColor[2] = 150;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;

    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    Effect_Add(globalCtx, &thisv->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    func_80A74714(thisv);

    if (thisv->switchFlags != 0xFF) {
        if (Flags_GetSwitch(globalCtx, thisv->switchFlags)) {
            Actor_Kill(thisx);
        }
    } else if (thisx->params != 0 && Flags_GetClear(globalCtx, globalCtx->roomCtx.curRoom.num)) {
        Actor_Kill(thisx);
    }
}

s32 func_80A745E4(EnIk* thisv, GlobalContext* globalCtx) {
    if (((thisv->unk_2FB != 0) || (thisv->actor.params == 0)) &&
        (func_800354B4(globalCtx, &thisv->actor, 100.0f, 0x2710, 0x4000, thisv->actor.shape.rot.y) != 0) &&
        (globalCtx->gameplayFrames & 1)) {
        func_80A755F0(thisv);
        return true;
    } else {
        return false;
    }
}

Actor* func_80A74674(GlobalContext* globalCtx, Actor* actor) {
    Actor* prop = globalCtx->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (prop != NULL) {
        if ((prop == actor) || (prop->id != ACTOR_BG_JYA_IRONOBJ)) {
            prop = prop->next;
            continue;
        } else if (Actor_ActorAIsFacingAndNearActorB(actor, prop, 80.0f, 0x2710)) {
            return prop;
        }

        prop = prop->next;
    }

    return NULL;
}

void func_80A74714(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_00CD70);
    f32 frame;

    if (thisv->actor.params >= 2) {
        frame = frames - 1.0f;
    } else {
        frame = 0.0f;
    }

    Animation_Change(&thisv->skelAnime, &object_ik_Anim_00CD70, 0.0f, frame, frames, ANIMMODE_ONCE, 0.0f);
    thisv->unk_2F8 = 3;
    thisv->actor.speedXZ = 0.0f;
    EnIk_SetupAction(thisv, func_80A747C0);
}

void func_80A747C0(EnIk* thisv, GlobalContext* globalCtx) {
    Vec3f sp24;

    if (thisv->bodyCollider.base.acFlags & AC_HIT) {
        sp24 = thisv->actor.world.pos;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_ARMOR_HIT);
        sp24.y += 30.0f;
        func_8003424C(globalCtx, &sp24);
        thisv->skelAnime.playSpeed = 1.0f;
        func_800F5ACC(NA_BGM_MINI_BOSS);
    }
    if (thisv->skelAnime.curFrame == 5.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_WAKEUP);
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2;
        func_80A74AAC(thisv);
    }
}

void func_80A7489C(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_00DD50);

    thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2;
    thisv->unk_2F8 = 4;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_00DD50, 0.0f, 0.0f, frames, ANIMMODE_LOOP, 4.0f);
    EnIk_SetupAction(thisv, func_80A7492C);
}

void func_80A7492C(EnIk* thisv, GlobalContext* globalCtx) {
    s32 phi_a0 = (thisv->unk_2FB == 0) ? 0xAAA : 0x3FFC;
    s16 yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if ((ABS(yawDiff) <= phi_a0) && (thisv->actor.xzDistToPlayer < 100.0f) &&
        (ABS(thisv->actor.yDistToPlayer) < 150.0f)) {
        if ((globalCtx->gameplayFrames & 1)) {
            func_80A74E2C(thisv);
        } else {
            func_80A751C8(thisv);
        }
    } else if ((ABS(yawDiff) <= 0x4000) && (ABS(thisv->actor.yDistToPlayer) < 150.0f)) {
        func_80A74AAC(thisv);
    } else {
        func_80A74AAC(thisv);
    }
    func_80A745E4(thisv, globalCtx);
    SkelAnime_Update(&thisv->skelAnime);
}

void func_80A74AAC(EnIk* thisv) {
    thisv->unk_2F8 = 5;
    if (thisv->unk_2FB == 0) {
        Animation_Change(&thisv->skelAnime, &object_ik_Anim_00ED24, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_ik_Anim_00ED24), ANIMMODE_LOOP, -4.0f);
        thisv->actor.speedXZ = 0.9f;
    } else {
        Animation_Change(&thisv->skelAnime, &object_ik_Anim_006734, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_ik_Anim_006734), ANIMMODE_LOOP, -4.0f);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_DASH);
        thisv->actor.speedXZ = 2.5f;
    }
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnIk_SetupAction(thisv, func_80A74BA4);
}

void func_80A74BA4(EnIk* thisv, GlobalContext* globalCtx) {
    s16 temp_t0;
    s16 temp_a1;
    s16 yawDiff;
    s16 sp30;
    s16 sp2E;
    s16 phi_a3;

    if (thisv->unk_2FB == 0) {
        temp_t0 = 0xAAA;
        phi_a3 = 0x320;
        sp30 = 0;
        sp2E = 0x10;
    } else {
        temp_t0 = 0x3FFC;
        phi_a3 = 0x4B0;
        sp30 = 2;
        sp2E = 9;
    }
    temp_a1 = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
    if ((thisv->actor.bgCheckFlags & 8) && (ABS(temp_a1) >= 0x4000)) {
        temp_a1 = (thisv->actor.yawTowardsPlayer > 0) ? thisv->actor.wallYaw - 0x4000 : thisv->actor.wallYaw + 0x4000;
        Math_SmoothStepToS(&thisv->actor.world.rot.y, temp_a1, 1, phi_a3, 0);
    } else {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, phi_a3, 0);
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if ((ABS(yawDiff) <= temp_t0) && (thisv->actor.xzDistToPlayer < 100.0f)) {
        if (ABS(thisv->actor.yDistToPlayer) < 150.0f) {
            if ((globalCtx->gameplayFrames & 1)) {
                func_80A74E2C(thisv);
            } else {
                func_80A751C8(thisv);
            }
        }
    }
    if (func_80A74674(globalCtx, &thisv->actor) != NULL) {
        func_80A751C8(thisv);
        thisv->unk_2FC = 1;
    } else {
        temp_t0 = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        if (ABS(temp_t0) > 0x4000) {
            thisv->unk_300--;
            if (thisv->unk_300 == 0) {
                func_80A754A0(thisv);
            }
        } else {
            thisv->unk_300 = 0x28;
        }
    }
    func_80A745E4(thisv, globalCtx);
    SkelAnime_Update(&thisv->skelAnime);
    if ((sp30 == (s16)thisv->skelAnime.curFrame) || (sp2E == (s16)thisv->skelAnime.curFrame)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_WALK);
    }
}

void func_80A74E2C(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_001C28);

    thisv->unk_2FF = 1;
    thisv->unk_2F8 = 6;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_001C28, 1.5f, 0.0f, frames, ANIMMODE_ONCE, -4.0f);
    EnIk_SetupAction(thisv, func_80A74EBC);
}

void func_80A74EBC(EnIk* thisv, GlobalContext* globalCtx) {
    Vec3f sp2C;

    if (thisv->skelAnime.curFrame == 15.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_SWING_AXE);
    } else if (thisv->skelAnime.curFrame == 21.0f) {
        sp2C.x = thisv->actor.world.pos.x + Math_SinS(thisv->actor.shape.rot.y + 0x6A4) * 70.0f;
        sp2C.z = thisv->actor.world.pos.z + Math_CosS(thisv->actor.shape.rot.y + 0x6A4) * 70.0f;
        sp2C.y = thisv->actor.world.pos.y;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_HIT_GND);
        Camera_AddQuake(&globalCtx->mainCamera, 2, 0x19, 5);
        func_800AA000(thisv->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
        CollisionCheck_SpawnShieldParticles(globalCtx, &sp2C);
    }

    if ((thisv->skelAnime.curFrame > 17.0f) && (thisv->skelAnime.curFrame < 23.0f)) {
        thisv->unk_2FE = 1;
    } else {
        if ((thisv->unk_2FB != 0) && (thisv->skelAnime.curFrame < 10.0f)) {
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x5DC, 0);
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        }
        thisv->unk_2FE = 0;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_80A7506C(thisv);
    }
}

void func_80A7506C(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_0029FC);

    thisv->unk_2FE = 0;
    thisv->unk_2F9 = (s8)frames;
    thisv->unk_2F8 = 7;
    thisv->unk_2FF = thisv->unk_2FE;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_0029FC, 1.0f, 0.0f, frames, ANIMMODE_LOOP, -4.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_PULLOUT);
    EnIk_SetupAction(thisv, func_80A7510C);
}

void func_80A7510C(EnIk* thisv, GlobalContext* globalCtx) {
    f32 frames;

    if (SkelAnime_Update(&thisv->skelAnime) || (--thisv->unk_2F9 == 0)) {
        if (thisv->unk_2F8 == 8) {
            func_80A7489C(thisv);
        } else {
            frames = Animation_GetLastFrame(&object_ik_Anim_002538);
            thisv->unk_2F8 = 8;
            Animation_Change(&thisv->skelAnime, &object_ik_Anim_002538, 1.5f, 0.0f, frames, ANIMMODE_ONCE_INTERP, -4.0f);
        }
    }
}

void func_80A751C8(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_0033C4);

    thisv->unk_2FF = 2;
    thisv->unk_300 = 0;
    thisv->unk_2F8 = 6;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_0033C4, 0.0f, 0.0f, frames, ANIMMODE_ONCE_INTERP, -6.0f);
    thisv->unk_2FC = 0;
    EnIk_SetupAction(thisv, func_80A75260);
}

void func_80A75260(EnIk* thisv, GlobalContext* globalCtx) {
    f32 temp_f0;

    thisv->unk_300 += 0x1C2;
    temp_f0 = Math_SinS(thisv->unk_300);
    thisv->skelAnime.playSpeed = ABS(temp_f0);

    if (thisv->skelAnime.curFrame > 11.0f) {
        thisv->unk_2FF = 3;
    }
    if (((thisv->skelAnime.curFrame > 1.0f) && (thisv->skelAnime.curFrame < 9.0f)) ||
        ((thisv->skelAnime.curFrame > 13.0f) && (thisv->skelAnime.curFrame < 18.0f))) {
        if ((thisv->unk_2FC == 0) && (thisv->unk_2FB != 0) && (thisv->skelAnime.curFrame < 10.0f)) {
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x5DC, 0);
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        }
        if (thisv->unk_2FE < 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_SWING_AXE);
        }
        thisv->unk_2FE = 1;
    } else {
        thisv->unk_2FE = 0;
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_80A753D0(thisv);
    }
}

void func_80A753D0(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_003DBC);

    thisv->unk_2FF = thisv->unk_2FE = 0;
    thisv->unk_2F8 = 8;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_003DBC, 1.5f, 0.0f, frames, ANIMMODE_ONCE_INTERP, -4.0f);
    EnIk_SetupAction(thisv, func_80A7545C);
}

void func_80A7545C(EnIk* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_80A7489C(thisv);
        func_80A745E4(thisv, globalCtx);
    }
}

void func_80A754A0(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_0033C4);

    thisv->unk_2F8 = 1;
    thisv->unk_2FF = 3;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_0033C4, 0.5f, 13.0f, frames, ANIMMODE_ONCE_INTERP, -4.0f);
    EnIk_SetupAction(thisv, func_80A75530);
}

void func_80A75530(EnIk* thisv, GlobalContext* globalCtx) {
    Math_StepUntilS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 0x7D0);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if ((thisv->skelAnime.curFrame > 13.0f) && (thisv->skelAnime.curFrame < 18.0f)) {
        if (thisv->unk_2FE < 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_SWING_AXE);
        }
        thisv->unk_2FE = 1;
    } else {
        thisv->unk_2FE = 0;
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_80A753D0(thisv);
        func_80A745E4(thisv, globalCtx);
    }
}

void func_80A755F0(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_00485C);

    thisv->unk_2FE = 0;
    thisv->unk_2F8 = 9;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_00485C, 1.0f, 0.0f, frames, ANIMMODE_ONCE_INTERP, -4.0f);
    EnIk_SetupAction(thisv, func_80A7567C);
}

void func_80A7567C(EnIk* thisv, GlobalContext* globalCtx) {
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->shieldCollider.base);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if ((ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x4000) &&
            (thisv->actor.xzDistToPlayer < 100.0f) && (ABS(thisv->actor.yDistToPlayer) < 150.0f)) {
            if ((globalCtx->gameplayFrames & 1)) {
                func_80A74E2C(thisv);
            } else {
                func_80A751C8(thisv);
            }
        } else {
            func_80A7489C(thisv);
        }
    }
}

void func_80A75790(EnIk* thisv) {
    s16 yaw;
    s16 yawDiff;

    yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->bodyCollider.base.ac->world.pos);
    thisv->unk_2F8 = 0;
    yawDiff = yaw - thisv->actor.shape.rot.y;
    if (ABS(yawDiff) <= 0x4000) {
        Animation_Change(&thisv->skelAnime, &object_ik_Anim_006194, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_ik_Anim_006194), ANIMMODE_ONCE, -4.0f);
        thisv->actor.speedXZ = -6.0f;
    } else {
        Animation_Change(&thisv->skelAnime, &object_ik_Anim_0045BC, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_ik_Anim_0045BC), ANIMMODE_ONCE, -4.0f);
        thisv->actor.speedXZ = 6.0f;
    }
    thisv->unk_2FE = 0;
    EnIk_SetupAction(thisv, func_80A758B0);
}

void func_80A758B0(EnIk* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    if (BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, thisv->actor.params + 4)) {
        thisv->bodyBreak.val = BODYBREAK_STATUS_FINISHED;
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x4000) {
            func_80A7489C(thisv);
            func_80A745E4(thisv, globalCtx);
        } else {
            func_80A754A0(thisv);
        }
    }
}

void func_80A7598C(EnIk* thisv) {
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_005944);

    thisv->unk_2FE = 0;
    thisv->unk_2F8 = 2;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_005944, 1.0f, 0.0f, frames, ANIMMODE_ONCE, -4.0f);
    thisv->unk_2F9 = 0x18;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_DEAD);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_CUTBODY);
    EnIk_SetupAction(thisv, func_80A75A38);
}

void func_80A75A38(EnIk* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if ((thisv->actor.colChkInfo.health == 0) && (thisv->unk_2F9 != 0)) {
            s32 i;
            Vec3f pos;
            Vec3f sp7C = { 0.0f, 0.5f, 0.0f };

            thisv->unk_2F9--;

            for (i = 0xC - (thisv->unk_2F9 >> 1); i >= 0; i--) {
                pos.x = thisv->actor.world.pos.x + Rand_CenteredFloat(120.0f);
                pos.z = thisv->actor.world.pos.z + Rand_CenteredFloat(120.0f);
                pos.y = thisv->actor.world.pos.y + 20.0f + Rand_CenteredFloat(50.0f);
                EffectSsDeadDb_Spawn(globalCtx, &pos, &sp7C, &sp7C, 100, 0, 255, 255, 255, 255, 0, 0, 255, 1, 9, true);
            }
            if (thisv->unk_2F9 == 0) {
                Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xB0);
                if (thisv->switchFlags != 0xFF) {
                    Flags_SetSwitch(globalCtx, thisv->switchFlags);
                }
                Actor_Kill(&thisv->actor);
            }
        }
    } else if (thisv->skelAnime.curFrame == 23.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_WALK);
    }
}

void func_80A75C38(EnIk* thisv, GlobalContext* globalCtx) {
    f32 temp_f0;
    u8 pad;
    u8 pad2;
    u8 prevHealth;
    s32 temp_v0_3;
    Vec3f sp38;

    if ((thisv->unk_2F8 == 3) || (thisv->unk_2F8 == 2)) {
        return;
    }
    if (thisv->shieldCollider.base.acFlags & AC_BOUNCED) {
        temp_f0 = Animation_GetLastFrame(&object_ik_Anim_00485C) - 2.0f;
        if (thisv->skelAnime.curFrame < temp_f0) {
            thisv->skelAnime.curFrame = temp_f0;
        }
        thisv->shieldCollider.base.acFlags &= ~AC_BOUNCED;
        thisv->bodyCollider.base.acFlags &= ~AC_HIT;
        return;
    }
    if (!(thisv->bodyCollider.base.acFlags & AC_HIT)) {
        return;
    }
    sp38 = thisv->actor.world.pos;
    sp38.y += 50.0f;
    Actor_SetDropFlag(&thisv->actor, &thisv->bodyCollider.info, 1);
    temp_v0_3 = thisv->actor.colChkInfo.damageEffect;
    thisv->unk_2FD = temp_v0_3 & 0xFF;
    thisv->bodyCollider.base.acFlags &= ~AC_HIT;

    if (1) {}

    if ((thisv->unk_2FD == 0) || (thisv->unk_2FD == 0xD) || ((thisv->unk_2FB == 0) && (thisv->unk_2FD == 0xE))) {
        if (thisv->unk_2FD != 0) {
            CollisionCheck_SpawnShieldParticlesMetal(globalCtx, &sp38);
        }
        return;
    }
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0xC);
    prevHealth = thisv->actor.colChkInfo.health;
    Actor_ApplyDamage(&thisv->actor);
    if (thisv->actor.params != 0) {
        if ((prevHealth > 10) && (thisv->actor.colChkInfo.health <= 10)) {
            thisv->unk_2FB = 1;
            BodyBreak_Alloc(&thisv->bodyBreak, 3, globalCtx);
        }
    } else if (thisv->actor.colChkInfo.health <= 10) {
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_BOSS);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EN_LAST_DAMAGE);
        if (thisv->switchFlags != 0xFF) {
            Flags_SetSwitch(globalCtx, thisv->switchFlags);
        }
        return;
    } else if (prevHealth == 50) {
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
    }

    if (thisv->actor.colChkInfo.health == 0) {
        func_80A7598C(thisv);
        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
        return;
    }
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x7D0, 0);
    if ((thisv->actor.params == 0) && (Rand_ZeroOne() < 0.5f)) {
        if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) > 0x4000) {
            func_80A754A0(thisv);
        }
    }
    if ((thisv->actor.params != 0) && (thisv->unk_2FB != 0)) {
        if ((prevHealth > 10) && (thisv->actor.colChkInfo.health <= 10)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_ARMOR_OFF_DEMO);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_DAMAGE);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_CUTBODY);
        }
        func_80A75790(thisv);
        return;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_ARMOR_HIT);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_IRONNACK_DAMAGE);
    CollisionCheck_SpawnShieldParticles(globalCtx, &sp38);
}

void func_80A75FA0(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    u8 prevInvincibilityTimer;

    thisv->unk_2FA = thisv->unk_2FB;
    func_80A75C38(thisv, globalCtx);
    if ((thisv->actor.params == 0) && (thisv->actor.colChkInfo.health <= 10)) {
        func_80A781CC(&thisv->actor, globalCtx);
        return;
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->axeCollider.base.atFlags & AT_HIT) {
        thisv->axeCollider.base.atFlags &= ~AT_HIT;
        if (&player->actor == thisv->axeCollider.base.at) {
            prevInvincibilityTimer = player->invincibilityTimer;
            if (player->invincibilityTimer <= 0) {
                if (player->invincibilityTimer < -39) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    globalCtx->damagePlayer(globalCtx, -64);
                    thisv->unk_2FE = 0;
                }
            }
            func_8002F71C(globalCtx, &thisv->actor, 8.0f, thisv->actor.yawTowardsPlayer, 8.0f);
            player->invincibilityTimer = prevInvincibilityTimer;
        }
    }
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 0x1D);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 45.0f;
    Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
    if ((thisv->actor.colChkInfo.health > 0) && (thisv->actor.colorFilterTimer == 0) && (thisv->unk_2F8 >= 2)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
    }
    if (thisv->unk_2FE > 0) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->axeCollider.base);
    }
    if (thisv->unk_2F8 == 9) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->shieldCollider.base);
    }
}

Gfx* func_80A761B0(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* displayList;
    Gfx* displayListHead;

    displayList = static_cast<Gfx*>(Graph_Alloc(gfxCtx, 4 * sizeof(Gfx)));
    displayListHead = displayList;

    gDPPipeSync(displayListHead++);
    gDPSetPrimColor(displayListHead++, 0, 0, primR, primG, primB, 255);
    gDPSetEnvColor(displayListHead++, envR, envG, envB, 255);
    gSPEndDisplayList(displayListHead++);

    return displayList;
}

s32 EnIk_OverrideLimbDraw3(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIk* thisv = (EnIk*)thisx;

    if (limbIndex == 12) {
        if (thisv->actor.params != 0) {
            *dList = object_ik_DL_018E78;
        }
    } else if (limbIndex == 13) {
        if (thisv->actor.params != 0) {
            *dList = object_ik_DL_019100;
        }
    } else if ((limbIndex == 26) || (limbIndex == 27)) {
        if ((thisv->unk_2FA & 1)) {
            *dList = NULL;
        }
    } else if ((limbIndex == 28) || (limbIndex == 29)) {
        if (!(thisv->unk_2FA & 1)) {
            *dList = NULL;
        }
    }
    return false;
}

// unused
static Vec3f D_80A78470 = { 300.0f, 0.0f, 0.0f };

static Vec3f D_80A7847C[] = {
    { 800.0f, -200.0f, -5200.0f },
    { 0.0f, 0.0f, 0.0f },
    { -200.0f, -2200.0f, -200.0f },
    { -6000.0f, 2000.0f, -3000.0f },
};

static Vec3f D_80A784AC[] = {
    { -3000.0, -700.0, -5000.0 },
    { -3000.0, -700.0, 2000.0 },
    { 4000.0, -700.0, 2000.0 },
};

static Vec3f D_80A784D0[] = {
    { 4000.0, -700.0, 2000.0 },
    { 4000.0, -700.0, -5000.0 },
    { -3000.0, -700.0, -5000.0 },
};

void EnIk_PostLimbDraw3(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f spF4;
    Vec3f spE8;
    EnIk* thisv = (EnIk*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1201);

    if (thisv->unk_2FB & 1) {
        BodyBreak_SetInfo(&thisv->bodyBreak, limbIndex, 26, 27, 28, dList, BODYBREAK_OBJECT_DEFAULT);
    }
    if (limbIndex == 12) {
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1217),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (thisv->actor.params != 0) {
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_019E08);
        } else {
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016D88);
        }
    } else if (limbIndex == 17) {
        s32 i;
        Vec3f sp9C[3];
        Vec3f sp78[3];

        Matrix_MultVec3f(&D_80A7847C[0], &thisv->axeCollider.dim.quad[1]);
        Matrix_MultVec3f(&D_80A7847C[1], &thisv->axeCollider.dim.quad[0]);
        Matrix_MultVec3f(&D_80A7847C[2], &thisv->axeCollider.dim.quad[3]);
        Matrix_MultVec3f(&D_80A7847C[3], &thisv->axeCollider.dim.quad[2]);
        Collider_SetQuadVertices(&thisv->axeCollider, &thisv->axeCollider.dim.quad[0], &thisv->axeCollider.dim.quad[1],
                                 &thisv->axeCollider.dim.quad[2], &thisv->axeCollider.dim.quad[3]);
        Matrix_MultVec3f(&D_80A7847C[0], &spF4);
        Matrix_MultVec3f(&D_80A7847C[1], &spE8);
        if (thisv->unk_2FE > 0) {
            EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blureIdx)), &spF4, &spE8);
        } else if (thisv->unk_2FE == 0) {
            EffectBlure_AddSpace(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blureIdx)));
            thisv->unk_2FE = -1;
        }
        if (thisv->unk_2F8 == 9) {
            for (i = 0; i < ARRAY_COUNT(sp78); i++) {
                Matrix_MultVec3f(&D_80A784AC[i], &sp9C[i]);
                Matrix_MultVec3f(&D_80A784D0[i], &sp78[i]);
            }

            Collider_SetTrisVertices(&thisv->shieldCollider, 0, &sp9C[0], &sp9C[1], &sp9C[2]);
            Collider_SetTrisVertices(&thisv->shieldCollider, 1, &sp78[0], &sp78[1], &sp78[2]);
        }
    }

    switch (limbIndex) {
        case 22:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1270),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
            break;
        case 24:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1275),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016EE8);
            break;
        case 26:
            if (!(thisv->unk_2FA & 1)) {
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1281),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016BE0);
            }
            break;
        case 27:
            if (!(thisv->unk_2FA & 1)) {
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1288),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
            }
            break;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1294);
}

void func_80A76798(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1309);

    func_80093D18(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);

    if (thisv->actor.params == 0) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80A761B0(globalCtx->state.gfxCtx, 245, 225, 155, 30, 30, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80A761B0(globalCtx->state.gfxCtx, 255, 40, 0, 40, 0, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80A761B0(globalCtx->state.gfxCtx, 255, 255, 255, 20, 40, 30));
    } else if (thisv->actor.params == 1) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80A761B0(globalCtx->state.gfxCtx, 245, 255, 205, 30, 35, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80A761B0(globalCtx->state.gfxCtx, 185, 135, 25, 20, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80A761B0(globalCtx->state.gfxCtx, 255, 255, 255, 30, 40, 20));
    } else if (thisv->actor.params == 2) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80A761B0(globalCtx->state.gfxCtx, 55, 65, 55, 0, 0, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80A761B0(globalCtx->state.gfxCtx, 205, 165, 75, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80A761B0(globalCtx->state.gfxCtx, 205, 165, 75, 25, 20, 0));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80A761B0(globalCtx->state.gfxCtx, 255, 255, 255, 180, 180, 180));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80A761B0(globalCtx->state.gfxCtx, 225, 205, 115, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80A761B0(globalCtx->state.gfxCtx, 225, 205, 115, 25, 20, 0));
    }
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnIk_OverrideLimbDraw3, EnIk_PostLimbDraw3, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ik_inFight.c", 1351);
}

void EnIk_StartMusic(void) {
    func_800F5ACC(NA_BGM_MINI_BOSS);
}

void func_80A76C14(EnIk* thisv) {
    if (Animation_OnFrame(&thisv->skelAnime, 1.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_WAKEUP, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (Animation_OnFrame(&thisv->skelAnime, 33.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_WALK, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (Animation_OnFrame(&thisv->skelAnime, 68.0f) || Animation_OnFrame(&thisv->skelAnime, 80.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_ARMOR_DEMO, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (Animation_OnFrame(&thisv->skelAnime, 107.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_FINGER_DEMO, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (Animation_OnFrame(&thisv->skelAnime, 156.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_ARMOR_DEMO, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (Animation_OnFrame(&thisv->skelAnime, 188.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_WAVE_DEMO, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void func_80A76DDC(EnIk* thisv, GlobalContext* globalCtx, Vec3f* pos) {
    Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_TRANSFORM, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);
}

void func_80A76E2C(EnIk* thisv, GlobalContext* globalCtx, Vec3f* pos) {
    static Vec3f D_80A78514[] = {
        { 1000.0, -1000.0, 1000.0 },  { 0.0, -1000.0, 0.0 },        { -1000.0, -5000.0, -4000.0 },
        { 1000.0, -5000.0, -3000.0 }, { -1000.0, 1000.0, -6000.0 }, { -1000.0, 3000.0, -5000.0 },
        { -800.0, 1000.0, -3000.0 },  { 0.0, -4000.0, -2000.0 },    { -1000.0, -2000.0, -6000.0 },
        { 1000.0, -3000.0, 0.0 },     { 2000.0, -2000.0, -4000.0 }, { -1000.0, 0.0, -6000.0 },
        { 1000.0, -2000.0, -2000.0 }, { 0.0, -2000.0, 2100.0 },     { 0.0, 0.0, 0.0 },
        { 1000.0, -1000.0, -6000.0 }, { 2000.0, 0.0, -3000.0 },     { -1000.0, -1000.0, -4000.0 },
        { 900.0, -800.0, 2700.0 },    { 720.0f, 900.0f, 2500.0f },
    };

    if (thisv->unk_4D4 == 0) {
        s32 pad;
        Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
        Vec3f effectAccel = { 0.0f, 0.3f, 0.0f };
        s32 i;

        for (i = ARRAY_COUNT(D_80A78514) - 1; i >= 0; i--) {
            Color_RGBA8 primColor = { 200, 200, 200, 255 };
            Color_RGBA8 envColor = { 150, 150, 150, 0 };
            s32 temp_v0;
            Vec3f effectPos;

            Matrix_MultVec3f(&D_80A78514[i], &effectPos);
            temp_v0 = (Rand_ZeroOne() * 20.0f) - 10.0f;
            primColor.r += temp_v0;
            primColor.g += temp_v0;
            primColor.b += temp_v0;
            envColor.r += temp_v0;
            envColor.g += temp_v0;
            envColor.b += temp_v0;
            func_8002829C(globalCtx, &effectPos, &effectVelocity, &effectAccel, &primColor, &envColor,
                          (Rand_ZeroOne() * 60.0f) + 300.0f, 0);
        }

        thisv->unk_4D4 = 1;
        func_80A76DDC(thisv, globalCtx, pos);
    }
}

void func_80A77034(EnIk* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 5);
}

s32 func_80A7707C(EnIk* thisv) {
    return SkelAnime_Update(&thisv->skelAnime);
}

const CsCmdActorAction* EnIk_GetNpcAction(GlobalContext* globalCtx, s32 actionIdx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        return globalCtx->csCtx.npcActions[actionIdx];
    } else {
        return NULL;
    }
}

void func_80A770C0(EnIk* thisv, GlobalContext* globalCtx, s32 actionIdx) {
    const CsCmdActorAction* npcAction = EnIk_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        thisv->actor.world.pos.x = npcAction->startPos.x;
        thisv->actor.world.pos.y = npcAction->startPos.y;
        thisv->actor.world.pos.z = npcAction->startPos.z;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = npcAction->rot.y;
    }
}

f32 EnIk_curFrame(Actor* thisx) {
    EnIk* thisv = (EnIk*)thisx;

    return thisv->skelAnime.curFrame;
}

void func_80A77148(EnIk* thisv) {
    thisv->action = 0;
    thisv->drawMode = 0;
    thisv->actor.shape.shadowAlpha = 0;
}

void func_80A77158(EnIk* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_00C114, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_ik_Anim_00C114), ANIMMODE_ONCE, 0.0f);
    func_80A770C0(thisv, globalCtx, 4);
    thisv->action = 1;
    thisv->drawMode = 1;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void func_80A771E4(EnIk* thisv) {
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_00C114, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_ik_Anim_00C114), ANIMMODE_ONCE, 0.0f);
    thisv->action = 2;
    thisv->drawMode = 1;
    thisv->unk_4D4 = 0;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void func_80A77264(EnIk* thisv, GlobalContext* globalCtx, s32 arg2) {
    if ((arg2 != 0) && (EnIk_GetNpcAction(globalCtx, 4) != NULL)) {
        func_80A78160(thisv, globalCtx);
    }
}

void func_80A772A4(EnIk* thisv) {
    Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_STAGGER_DEMO, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);
}

void func_80A772EC(EnIk* thisv, GlobalContext* globalCtx) {
    static Vec3f D_80A78FA0;
    s32 pad[2];
    f32 wDest;

    SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &thisv->actor.world.pos, &D_80A78FA0, &wDest);
    Audio_PlaySoundGeneral(NA_SE_EN_IRONNACK_DEAD, &D_80A78FA0, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void func_80A7735C(EnIk* thisv, GlobalContext* globalCtx) {
    s32 pad[3];
    f32 frames = Animation_GetLastFrame(&object_ik_Anim_0203D8);

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ik_Skel_0205C0, NULL, thisv->jointTable, thisv->morphTable,
                       30);
    Animation_Change(&thisv->skelAnime, &object_ik_Anim_0203D8, 1.0f, 0.0f, frames, ANIMMODE_ONCE, 0.0f);
    thisv->action = 3;
    thisv->drawMode = 2;
    func_80A770C0(thisv, globalCtx, 4);
    func_80A772EC(thisv, globalCtx);
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void func_80A77434(EnIk* thisv, GlobalContext* globalCtx) {
    thisv->action = 4;
    thisv->drawMode = 2;
    func_80A772A4(thisv);
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void func_80A77474(EnIk* thisv, GlobalContext* globalCtx) {
    thisv->action = 5;
    thisv->drawMode = 0;
    thisv->actor.shape.shadowAlpha = 0;
}

void func_80A7748C(EnIk* thisv, GlobalContext* globalCtx) {
    func_80A77034(thisv, globalCtx);
    func_80A779DC(thisv, globalCtx);
}

void func_80A774BC(EnIk* thisv, GlobalContext* globalCtx) {
    func_80A7707C(thisv);
    func_80A77034(thisv, globalCtx);
    func_80A779DC(thisv, globalCtx);
}

void func_80A774F8(EnIk* thisv, GlobalContext* globalCtx) {
    if (EnIk_GetNpcAction(globalCtx, 4) == NULL) {
        Actor_Kill(&thisv->actor);
    }
}

s32 EnIk_OverrideLimbDraw2(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIk* thisv = (EnIk*)thisx;

    if ((limbIndex == 13) || (limbIndex == 26) || (limbIndex == 27)) {
        if (EnIk_curFrame(&thisv->actor) >= 30.0f) {
            *dList = NULL;
        }
    }

    return 0;
}

void EnIk_PostLimbDraw2(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 207);

    switch (limbIndex) {
        case 13: {
            EnIk* thisv = (EnIk*)thisx;

            if (EnIk_curFrame(&thisv->actor) < 30.0f) {
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inAwake.c", 267),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016D88);
            }
        } break;
        case 22:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inAwake.c", 274),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
            break;
        case 24:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inAwake.c", 280),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016EE8);
            break;
        case 26: {
            EnIk* thisv = (EnIk*)thisx;

            if (EnIk_curFrame(&thisv->actor) < 30.0f) {
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inAwake.c", 288),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016BE0);
            }
        } break;
        case 27: {
            EnIk* thisv = (EnIk*)thisx;

            if (EnIk_curFrame(&thisv->actor) < 30.0f) {
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inAwake.c", 297),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
            }
        } break;
    }
    CLOSE_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 304);
}

void func_80A77844(EnIk* thisv, GlobalContext* globalCtx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    SkelAnime* skelAnime = &thisv->skelAnime;
    s32 pad[2];

    OPEN_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 322);

    func_8002EBCC(&thisv->actor, globalCtx, 0);
    func_80093D18(gfxCtx);
    func_80093D84(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, func_80A761B0(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, func_80A761B0(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, func_80A761B0(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnIk_OverrideLimbDraw2, EnIk_PostLimbDraw2, thisv);

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inAwake.c", 345);
}

void func_80A779DC(EnIk* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* npcAction = EnIk_GetNpcAction(globalCtx, 4);
    u32 action;
    u32 currentNpcAction;

    if (npcAction != NULL) {
        action = npcAction->action;
        currentNpcAction = thisv->npcAction;
        if (action != currentNpcAction) {
            switch (action) {
                case 1:
                    func_80A77148(thisv);
                    break;
                case 2:
                    func_80A77158(thisv, globalCtx);
                    break;
                case 3:
                    func_80A771E4(thisv);
                    break;
                case 4:
                    func_80A78160(thisv, globalCtx);
                    break;
                case 5:
                    func_80A7735C(thisv, globalCtx);
                    break;
                case 6:
                    func_80A77434(thisv, globalCtx);
                    break;
                case 7:
                    func_80A77474(thisv, globalCtx);
                    break;
                default:
                    osSyncPrintf("En_Ik_inConfrontion_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }

            thisv->npcAction = action;
        }
    }
}

void func_80A77AEC(EnIk* thisv, GlobalContext* globalCtx) {
    func_80A779DC(thisv, globalCtx);
}

void func_80A77B0C(EnIk* thisv, GlobalContext* globalCtx) {
    func_80A77034(thisv, globalCtx);
    func_80A779DC(thisv, globalCtx);
}

void func_80A77B3C(EnIk* thisv, GlobalContext* globalCtx) {
    s32 sp24;

    sp24 = func_80A7707C(thisv);
    func_80A76C14(thisv);
    func_80A77034(thisv, globalCtx);
    func_80A779DC(thisv, globalCtx);
    func_80A77264(thisv, globalCtx, sp24);
}

static EnIkActionFunc sActionFuncs[] = {
    func_80A77AEC, func_80A77B0C, func_80A77B3C, func_80A7748C, func_80A774BC, func_80A774F8,
};

void EnIk_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;

    if (thisv->action < 0 || thisv->action >= ARRAY_COUNT(sActionFuncs) || sActionFuncs[thisv->action] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sActionFuncs[thisv->action](thisv, globalCtx);
}

s32 EnIk_OverrideLimbDraw1(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIk* thisv = (EnIk*)thisx;
    f32 curFrame;

    switch (limbIndex) {
        case 17:
            curFrame = thisv->skelAnime.curFrame;
            if (curFrame < 120.0f) {
                *dList = NULL;
            } else {
                func_80A76E2C(thisv, globalCtx, pos);
            }
            break;
        case 29:
        case 30:
            *dList = NULL;
            break;
    }

    return 0;
}

void EnIk_PostLimbDraw1(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 571);

    switch (limbIndex) {
        case 12:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inConfrontion.c", 575),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016D88);
            break;
        case 22:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inConfrontion.c", 581),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016F88);
            break;
        case 24:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inConfrontion.c", 587),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016EE8);
            break;
        case 26:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inConfrontion.c", 593),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016BE0);
            break;
        case 27:
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_ik_inConfrontion.c", 599),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_ik_DL_016CD8);
            break;
    }

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 604);
}

void func_80A77ED0(EnIk* thisv, GlobalContext* globalCtx) {
}

void func_80A77EDC(EnIk* thisv, GlobalContext* globalCtx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    SkelAnime* skelAnime = &thisv->skelAnime;
    s32 pad[2];

    OPEN_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 630);

    func_8002EBCC(&thisv->actor, globalCtx, 0);
    func_80093D18(gfxCtx);
    func_80093D84(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, func_80A761B0(gfxCtx, 245, 225, 155, 30, 30, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, func_80A761B0(gfxCtx, 255, 40, 0, 40, 0, 0));
    gSPSegment(POLY_OPA_DISP++, 0x0A, func_80A761B0(gfxCtx, 255, 255, 255, 20, 40, 30));
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnIk_OverrideLimbDraw1, EnIk_PostLimbDraw1, thisv);

    CLOSE_DISPS(gfxCtx, "../z_en_ik_inConfrontion.c", 653);
}

static EnIkDrawFunc sDrawFuncs[] = { func_80A77ED0, func_80A77EDC, func_80A77844 };

void EnIk_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;

    if (thisv->drawMode < 0 || thisv->drawMode >= ARRAY_COUNT(sDrawFuncs) || sDrawFuncs[thisv->drawMode] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sDrawFuncs[thisv->drawMode](thisv, globalCtx);
}

void func_80A780D0(EnIk* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params == 0) {
        if (!(gSaveContext.eventChkInf[3] & 0x800)) {
            thisv->actor.update = EnIk_Update;
            thisv->actor.draw = EnIk_Draw;
            Actor_SetScale(&thisv->actor, 0.01f);
        } else {
            func_80A78160(thisv, globalCtx);
            EnIk_StartMusic();
        }
    }
    osSyncPrintf("En_Ik_inConfrontion_Init : %d !!!!!!!!!!!!!!!!\n", thisv->actor.params);
}

void func_80A78160(EnIk* thisv, GlobalContext* globalCtx) {
    thisv->actor.update = func_80A75FA0;
    thisv->actor.draw = func_80A76798;
    thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2;
    gSaveContext.eventChkInf[3] |= 0x800;
    Actor_SetScale(&thisv->actor, 0.012f);
    func_80A7489C(thisv);
}

void func_80A781CC(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;

    if (!Gameplay_InCsMode(globalCtx)) {
        thisv->actor.update = EnIk_Update;
        thisv->actor.draw = EnIk_Draw;
        Cutscene_SetSegment(globalCtx, gSpiritBossNabooruKnuckleDefeatCs);
        gSaveContext.cutsceneTrigger = 1;
        Actor_SetScale(&thisv->actor, 0.01f);
        gSaveContext.eventChkInf[3] |= 0x1000;
        func_80A7735C(thisv, globalCtx);
    }
}

void EnIk_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnIk* thisv = (EnIk*)thisx;
    s32 flag = thisv->actor.params & 0xFF00;

    if (((thisv->actor.params & 0xFF) == 0 && (gSaveContext.eventChkInf[3] & 0x1000)) ||
        (flag != 0 && Flags_GetSwitch(globalCtx, flag >> 8))) {
        Actor_Kill(&thisv->actor);
    } else {
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ik_Skel_01E178, &object_ik_Anim_00C114,
                           thisv->jointTable, thisv->morphTable, 30);
        func_80A74398(&thisv->actor, globalCtx);
        func_80A780D0(thisv, globalCtx);
    }
}

ActorInit En_Ik_InitVars = {
    ACTOR_EN_IK,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_IK,
    sizeof(EnIk),
    (ActorFunc)EnIk_Init,
    (ActorFunc)EnIk_Destroy,
    (ActorFunc)EnIk_Update,
    (ActorFunc)EnIk_Draw,
    NULL,
};
