/*
 * File: z_en_dodojr.c
 * Overlay: ovl_En_Dodojr
 * Description: Baby Dodongo
 */

#include "z_en_dodojr.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/object_dodojr/object_dodojr.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnDodojr_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDodojr_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDodojr_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDodojr_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809F73AC(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F7BE4(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F74C4(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F758C(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F786C(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F799C(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F78EC(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F773C(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F77AC(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F784C(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F7AB8(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F7A00(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F7B3C(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F7C48(EnDodojr* thisv, GlobalContext* globalCtx);
void func_809F768C(EnDodojr* thisv, GlobalContext* globalCtx);

const ActorInit En_Dodojr_InitVars = {
    ACTOR_EN_DODOJR,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DODOJR,
    sizeof(EnDodojr),
    (ActorFunc)EnDodojr_Init,
    (ActorFunc)EnDodojr_Destroy,
    (ActorFunc)EnDodojr_Update,
    (ActorFunc)EnDodojr_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFC5FFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 20, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 1, 2, 25, 25, 0xFF };

void EnDodojr_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDodojr* thisv = (EnDodojr*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 18.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_dodojr_Skel_0020E0, &object_dodojr_Anim_0009D4,
                   thisv->jointTable, thisv->morphTable, 15);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(4), &sColChkInit);

    thisv->actor.naviEnemyId = 0xE;
    thisv->actor.flags &= ~ACTOR_FLAG_0;

    Actor_SetScale(&thisv->actor, 0.02f);

    thisv->actionFunc = func_809F73AC;
}

void EnDodojr_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDodojr* thisv = (EnDodojr*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_809F64D0(EnDodojr* thisv) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_BOMB_EXPLOSION);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 200, 0, 8);
}

void func_809F6510(EnDodojr* thisv, GlobalContext* globalCtx, s32 count) {
    Color_RGBA8 prim = { 170, 130, 90, 255 };
    Color_RGBA8 env = { 100, 60, 20, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = ((Rand_ZeroOne() - 0.5f) * 65536.0f);
    s32 i;

    pos.y = thisv->dustPos.y;

    for (i = count; i >= 0; i--, angle += (s16)(0x10000 / count)) {
        accel.x = (Rand_ZeroOne() - 0.5f) * 4.0f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 4.0f;

        pos.x = (Math_SinS(angle) * 22.0f) + thisv->dustPos.x;
        pos.z = (Math_CosS(angle) * 22.0f) + thisv->dustPos.z;

        func_8002836C(globalCtx, &pos, &velocity, &accel, &prim, &env, 120, 40, 10);
    }
}

void func_809F6730(EnDodojr* thisv, GlobalContext* globalCtx, Vec3f* arg2) {
    Color_RGBA8 prim = { 170, 130, 90, 255 };
    Color_RGBA8 env = { 100, 60, 20, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = ((Rand_ZeroOne() - 0.5f) * 65536.0f);

    pos.y = thisv->actor.floorHeight;

    accel.x = (Rand_ZeroOne() - 0.5f) * 2;
    accel.z = (Rand_ZeroOne() - 0.5f) * 2;

    pos.x = (Math_SinS(angle) * 11.0f) + arg2->x;
    pos.z = (Math_CosS(angle) * 11.0f) + arg2->z;

    func_8002836C(globalCtx, &pos, &velocity, &accel, &prim, &env, 100, 60, 8);
}

s32 func_809F68B0(EnDodojr* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.velocity.y >= 0.0f) {
        return 0;
    }

    if (thisv->unk_1FC == 0) {
        return 0;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        thisv->dustPos = thisv->actor.world.pos;
        func_809F6510(thisv, globalCtx, 10);
        thisv->actor.velocity.y = 10.0f / (4 - thisv->unk_1FC);
        thisv->unk_1FC--;

        if (thisv->unk_1FC == 0) {
            thisv->actor.velocity.y = 0.0f;
            return 1;
        }
    }

    return 0;
}

void func_809F6994(EnDodojr* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_000860);

    Animation_Change(&thisv->skelAnime, &object_dodojr_Anim_000860, 1.8f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -10.0f);
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.speedXZ = 2.6f;
    thisv->actor.gravity = -0.8f;
}

void func_809F6A20(EnDodojr* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_0004A0);

    Animation_Change(&thisv->skelAnime, &object_dodojr_Anim_0004A0, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -10.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.x = 0.0f;
    thisv->actor.velocity.z = 0.0f;
    thisv->actor.gravity = -0.8f;

    if (thisv->unk_1FC == 0) {
        thisv->unk_1FC = 3;
        thisv->actor.velocity.y = 10.0f;
    }
}

void func_809F6AC4(EnDodojr* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_0005F0);

    Animation_Change(&thisv->skelAnime, &object_dodojr_Anim_0005F0, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, 0.0f);
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = -0.8f;
}

void func_809F6B38(EnDodojr* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_000724);

    Animation_Change(&thisv->skelAnime, &object_dodojr_Anim_000724, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actor.gravity = -0.8f;
    thisv->unk_1FC = 3;
    thisv->actor.velocity.y = 10.0f;
}

void func_809F6BBC(EnDodojr* thisv) {
    thisv->actor.shape.shadowDraw = NULL;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.home.pos = thisv->actor.world.pos;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = -0.8f;
    thisv->timer3 = 30;
    thisv->dustPos = thisv->actor.world.pos;
}

void func_809F6C24(EnDodojr* thisv) {
    Animation_Change(&thisv->skelAnime, &object_dodojr_Anim_000724, 1.0f, 8.0f, 12.0f, ANIMMODE_ONCE, 0.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_EAT);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.x = 0.0f;
    thisv->actor.velocity.z = 0.0f;
    thisv->actor.gravity = -0.8f;
}

s32 func_809F6CA4(EnDodojr* thisv, GlobalContext* globalCtx) {
    Actor* bomb;
    Vec3f unkVec = { 99999.0f, 99999.0f, 99999.0f };
    s32 retVar = 0;
    f32 xDist;
    f32 yDist;
    f32 zDist;

    bomb = globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    thisv->bomb = NULL;

    while (bomb != NULL) {
        if ((bomb->params != 0) || (bomb->parent != NULL) || (bomb->update == NULL)) {
            bomb = bomb->next;
            continue;
        }

        if (bomb->id != ACTOR_EN_BOM) {
            bomb = bomb->next;
            continue;
        }

        xDist = bomb->world.pos.x - thisv->actor.world.pos.x;
        yDist = bomb->world.pos.y - thisv->actor.world.pos.y;
        zDist = bomb->world.pos.z - thisv->actor.world.pos.z;

        if ((fabsf(xDist) >= fabsf(unkVec.x)) || (fabsf(yDist) >= fabsf(unkVec.y)) ||
            (fabsf(zDist) >= fabsf(unkVec.z))) {
            bomb = bomb->next;
            continue;
        }

        thisv->bomb = bomb;
        unkVec = bomb->world.pos;
        retVar = 1;
        bomb = bomb->next;
    }

    return retVar;
}

s32 func_809F6DD0(EnDodojr* thisv) {
    if (thisv->bomb == NULL) {
        return 0;
    } else if (thisv->bomb->parent != NULL) {
        return 0;
    } else if (Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->bomb->world.pos) > 30.0f) {
        return 0;
    } else {
        thisv->bomb->parent = &thisv->actor;
        return 1;
    }
}

void func_809F6E54(EnDodojr* thisv, GlobalContext* globalCtx) {
    f32 angles[] = { 0.0f, 210.0f, 60.0f, 270.0f, 120.0f, 330.0f, 180.0f, 30.0f, 240.0f, 90.0f, 300.0f, 150.0f };
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f pos;
    s16 angleIndex;

    if ((thisv->bomb == NULL) || (thisv->bomb->update == NULL) ||
        ((thisv->bomb != NULL) && (thisv->bomb->parent != NULL))) {
        func_809F6CA4(thisv, globalCtx);
    }

    if (thisv->bomb != NULL) {
        pos = thisv->bomb->world.pos;
    } else {
        pos = player->actor.world.pos;
    }

    if (Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &pos) > 80.0f) {
        angleIndex = (s16)(thisv->actor.home.pos.x + thisv->actor.home.pos.y + thisv->actor.home.pos.z +
                           globalCtx->state.frames / 30) %
                     12;
        angleIndex = ABS(angleIndex);
        pos.x += 80.0f * sinf(angles[angleIndex]);
        pos.z += 80.0f * cosf(angles[angleIndex]);
    }

    Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_Vec3f_Yaw(&thisv->actor.world.pos, &pos), 10, 1000, 1);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

s32 func_809F706C(EnDodojr* thisv) {
    if (thisv->actor.xzDistToPlayer > 40.0f) {
        return 0;
    } else {
        return 1;
    }
}

void func_809F709C(EnDodojr* thisv) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_DEAD);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    func_809F6A20(thisv);
    thisv->actionFunc = func_809F7AB8;
}

s32 func_809F70E8(EnDodojr* thisv, GlobalContext* globalCtx) {
    if ((thisv->actionFunc == func_809F773C) || (thisv->actionFunc == func_809F77AC) ||
        (thisv->actionFunc == func_809F784C) || (thisv->actionFunc == func_809F7A00) ||
        (thisv->actionFunc == func_809F7AB8) || (thisv->actionFunc == func_809F7B3C) ||
        (thisv->actionFunc == func_809F7BE4)) {
        return 0;
    }

    if (globalCtx->actorCtx.unk_02 != 0) {
        if (thisv->actionFunc != func_809F73AC) {
            if (thisv->actionFunc == func_809F74C4) {
                thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
            }

            func_809F709C(thisv);
        }
        return 0;
    }

    if (!(thisv->collider.base.acFlags & 2)) {
        return 0;
    } else {
        thisv->collider.base.acFlags &= ~2;

        if ((thisv->actionFunc == func_809F73AC) || (thisv->actionFunc == func_809F74C4)) {
            thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        }

        if ((thisv->actor.colChkInfo.damageEffect == 0) && (thisv->actor.colChkInfo.damage != 0)) {
            Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
            thisv->timer2 = 2;
            thisv->actionFunc = func_809F7C48;
            return 1;
        }

        if ((thisv->actor.colChkInfo.damageEffect == 1) && (thisv->actionFunc != func_809F78EC) &&
            (thisv->actionFunc != func_809F786C)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
            thisv->timer1 = 120;
            Actor_SetColorFilter(&thisv->actor, 0, 200, 0, 120);
            func_809F6A20(thisv);
            thisv->actionFunc = func_809F786C;
        }

        return 0;
    }
}

void func_809F72A4(EnDodojr* thisv, GlobalContext* globalCtx) {
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    if ((thisv->actionFunc != func_809F73AC) && (thisv->actionFunc != func_809F7BE4)) {
        if ((thisv->actionFunc == func_809F74C4) || (thisv->actionFunc == func_809F758C) ||
            (thisv->actionFunc == func_809F799C)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        if ((thisv->actionFunc == func_809F74C4) || (thisv->actionFunc == func_809F758C) ||
            (thisv->actionFunc == func_809F786C) || (thisv->actionFunc == func_809F78EC) ||
            (thisv->actionFunc == func_809F799C)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void func_809F73AC(EnDodojr* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_000860);
    Player* player = GET_PLAYER(globalCtx);
    f32 dist;

    if (!(thisv->actor.xzDistToPlayer >= 320.0f)) {
        dist = thisv->actor.world.pos.y - player->actor.world.pos.y;

        if (!(dist >= 40.0f)) {
            Animation_Change(&thisv->skelAnime, &object_dodojr_Anim_000860, 1.8f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP,
                             -10.0f);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_UP);
            thisv->actor.world.pos.y -= 60.0f;
            thisv->actor.flags |= ACTOR_FLAG_0;
            thisv->actor.world.rot.x -= 0x4000;
            thisv->actor.shape.rot.x = thisv->actor.world.rot.x;
            thisv->dustPos = thisv->actor.world.pos;
            thisv->dustPos.y = thisv->actor.floorHeight;
            thisv->actionFunc = func_809F74C4;
        }
    }
}

void func_809F74C4(EnDodojr* thisv, GlobalContext* globalCtx) {
    f32 sp2C;

    Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0, 4, 0x3E8, 0x64);
    sp2C = thisv->actor.shape.rot.x;
    sp2C /= 16384.0f;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + (60.0f * sp2C);
    func_809F6510(thisv, globalCtx, 3);

    if (sp2C == 0.0f) {
        thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        thisv->actor.world.rot.x = thisv->actor.shape.rot.x;
        thisv->actor.speedXZ = 2.6f;
        thisv->actionFunc = func_809F758C;
    }
}

void func_809F758C(EnDodojr* thisv, GlobalContext* globalCtx) {
    func_8002D868(&thisv->actor);
    func_809F6730(thisv, globalCtx, &thisv->actor.world.pos);

    if (DECR(thisv->timer4) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_MOVE);
        thisv->timer4 = 5;
    }

    if (func_809F6DD0(thisv) != 0) {
        func_809F6C24(thisv);
        thisv->actionFunc = func_809F768C;
        return;
    }

    func_809F6E54(thisv, globalCtx);

    if (func_809F706C(thisv) != 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_CRY);
        func_809F6B38(thisv);
        thisv->actionFunc = func_809F799C;
    }

    if (thisv->actor.bgCheckFlags & 8) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_DOWN);
        func_809F6BBC(thisv);
        thisv->actionFunc = func_809F7A00;
    }
}

void func_809F768C(EnDodojr* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;

    if (((s16)thisv->skelAnime.curFrame - 8) < 4) {
        bomb = (EnBom*)thisv->bomb;
        bomb->timer++;
        thisv->bomb->world.pos = thisv->headPos;
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_DRINK);
        Actor_Kill(thisv->bomb);
        thisv->timer3 = 24;
        thisv->unk_1FC = 0;
        thisv->actionFunc = func_809F773C;
    }
}

void func_809F773C(EnDodojr* thisv, GlobalContext* globalCtx) {
    if (DECR(thisv->timer3) == 0) {
        func_809F64D0(thisv);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        func_809F6A20(thisv);
        thisv->actionFunc = func_809F77AC;
    }
}

void func_809F77AC(EnDodojr* thisv, GlobalContext* globalCtx) {
    thisv->rootScale = 1.2f;
    thisv->rootScale *= ((f32)thisv->actor.colorFilterTimer / 8);
    func_8002D868(&thisv->actor);

    if (func_809F68B0(thisv, globalCtx) != 0) {
        thisv->timer3 = 60;
        func_809F6AC4(thisv);
        thisv->unk_1FC = 7;
        thisv->actionFunc = func_809F784C;
    }
}

void func_809F784C(EnDodojr* thisv, GlobalContext* globalCtx) {
    func_809F7B3C(thisv, globalCtx);
}

void func_809F786C(EnDodojr* thisv, GlobalContext* globalCtx) {
    func_8002D868(&thisv->actor);

    if (func_809F68B0(thisv, globalCtx) != 0) {
        func_809F6AC4(thisv);
        thisv->actionFunc = func_809F78EC;
    }

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, 0, 4, 1000, 10);
    thisv->actor.world.rot.x = thisv->actor.shape.rot.x;
    thisv->actor.colorFilterTimer = thisv->timer1;
}

void func_809F78EC(EnDodojr* thisv, GlobalContext* globalCtx) {
    if (DECR(thisv->timer1) != 0) {
        if (thisv->timer1 < 30) {
            if ((thisv->timer1 & 1) != 0) {
                thisv->actor.world.pos.x += 1.5f;
                thisv->actor.world.pos.z += 1.5f;
            } else {
                thisv->actor.world.pos.x -= 1.5f;
                thisv->actor.world.pos.z -= 1.5f;
            }

            return;
        }
    } else {
        func_809F6994(thisv);
        thisv->actionFunc = func_809F758C;
    }
}

void func_809F799C(EnDodojr* thisv, GlobalContext* globalCtx) {
    thisv->actor.flags |= ACTOR_FLAG_24;
    func_8002D868(&thisv->actor);

    if (func_809F68B0(thisv, globalCtx) != 0) {
        func_809F6994(thisv);
        thisv->actionFunc = func_809F758C;
    }
}

void func_809F7A00(EnDodojr* thisv, GlobalContext* globalCtx) {
    f32 tmp;

    Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0x4000, 4, 1000, 100);

    if (DECR(thisv->timer3) != 0) {
        tmp = (30 - thisv->timer3) / 30.0f;
        thisv->actor.world.pos.y = thisv->actor.home.pos.y - (60.0f * tmp);
    } else {
        Actor_Kill(&thisv->actor);
    }

    func_809F6510(thisv, globalCtx, 3);
}

void func_809F7AB8(EnDodojr* thisv, GlobalContext* globalCtx) {
    func_8002D868(&thisv->actor);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, 0, 4, 1000, 10);
    thisv->actor.world.rot.x = thisv->actor.shape.rot.x;

    if (func_809F68B0(thisv, globalCtx) != 0) {
        thisv->timer3 = 60;
        func_809F6AC4(thisv);
        thisv->unk_1FC = 7;
        thisv->actionFunc = func_809F7B3C;
    }
}

void func_809F7B3C(EnDodojr* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;

    if (thisv->unk_1FC != 0) {
        if (thisv->actor.colorFilterTimer == 0) {
            Actor_SetColorFilter(&thisv->actor, 0x4000, 200, 0, thisv->unk_1FC);
            thisv->unk_1FC--;
        }
    } else {
        bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->actor.world.pos.x,
                                   thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        thisv->timer3 = 8;
        thisv->actionFunc = func_809F7BE4;
    }
}

void func_809F7BE4(EnDodojr* thisv, GlobalContext* globalCtx) {
    if (DECR(thisv->timer3) == 0) {
        Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, 0x40);
        Actor_Kill(&thisv->actor);
    }
}

void func_809F7C48(EnDodojr* thisv, GlobalContext* globalCtx) {
    if (DECR(thisv->timer2) == 0) {
        func_809F709C(thisv);
    }
}

void EnDodojr_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDodojr* thisv = (EnDodojr*)thisx;

    SkelAnime_Update(&thisv->skelAnime);
    Actor_MoveForward(&thisv->actor);
    func_809F70E8(thisv, globalCtx);

    if (thisv->actionFunc != func_809F73AC) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, thisv->collider.dim.radius, thisv->collider.dim.height, 0.0f, 5);
    }

    thisv->actionFunc(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, 10.0f);
    func_809F72A4(thisv, globalCtx);
}

s32 func_809F7D50(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDodojr* thisv = (EnDodojr*)thisx;
    Vec3f D_809F7F64 = { 480.0f, 620.0f, 0.0f };

    if (limbIndex == 1) {
        Matrix_Scale((thisv->rootScale * 0.5f) + 1.0f, thisv->rootScale + 1.0f, (thisv->rootScale * 0.5f) + 1.0f,
                     MTXMODE_APPLY);
    }

    if (limbIndex == 4) {
        Matrix_MultVec3f(&D_809F7F64, &thisv->headPos);
    }

    return false;
}

void func_809F7DFC(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void EnDodojr_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDodojr* thisv = (EnDodojr*)thisx;

    if ((thisv->actionFunc != func_809F73AC) && (thisv->actionFunc != func_809F7BE4)) {
        func_80093D18(globalCtx->state.gfxCtx);
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, func_809F7D50, func_809F7DFC,
                          &thisv->actor);
    }
}
