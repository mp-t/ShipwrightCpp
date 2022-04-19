/*
 * File: z_en_brob.c
 * Overlay: ovl_En_Brob
 * Description: Flobbery Muscle Block (Jabu-Jabu's Belly)
 */

#include "z_en_brob.h"
#include "objects/object_brob/object_brob.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnBrob_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBrob_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBrob_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBrob_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809CADDC(EnBrob* thisv, GlobalContext* globalCtx);
void func_809CB054(EnBrob* thisv, GlobalContext* globalCtx);
void func_809CB114(EnBrob* thisv, GlobalContext* globalCtx);
void func_809CB218(EnBrob* thisv, GlobalContext* globalCtx);
void func_809CB2B8(EnBrob* thisv, GlobalContext* globalCtx);
void func_809CB354(EnBrob* thisv, GlobalContext* globalCtx);
void func_809CB458(EnBrob* thisv, GlobalContext* globalCtx);

ActorInit En_Brob_InitVars = {
    ACTOR_EN_BROB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BROB,
    sizeof(EnBrob),
    (ActorFunc)EnBrob_Init,
    (ActorFunc)EnBrob_Destroy,
    (ActorFunc)EnBrob_Update,
    (ActorFunc)EnBrob_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0xFFCFFFFF, 0x03, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 8000, 11000, -5000, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 60, 120, MASS_IMMOVABLE };

void EnBrob_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnBrob* thisv = (EnBrob*)thisx;
    const CollisionHeader* colHeader = NULL;

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_brob_Skel_0015D8, &object_brob_Anim_001750,
                       thisv->jointTable, thisv->morphTable, 10);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&object_brob_Col_001A70, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
    Collider_InitCylinder(globalCtx, &thisv->colliders[0]);
    Collider_SetCylinder(globalCtx, &thisv->colliders[0], &thisv->dyna.actor, &sCylinderInit);
    Collider_InitCylinder(globalCtx, &thisv->colliders[1]);
    Collider_SetCylinder(globalCtx, &thisv->colliders[1], &thisv->dyna.actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sColChkInfoInit);
    if (((thisx->params >> 8) & 0xFF) == 0) {
        Actor_SetScale(&thisv->dyna.actor, 0.01f);
        thisx->params &= 0xFF;
        if (thisx->params != 0xFF) {
            thisx->scale.y *= (thisx->params & 0xFF) * (1.0f / 30.0f);
        }
    } else {
        Actor_SetScale(&thisv->dyna.actor, 0.005f);
        thisx->params &= 0xFF;
        if (thisx->params != 0xFF) {
            thisx->scale.y *= (thisx->params & 0xFF) * (2.0f / 30.0f);
        }
    }
    thisv->colliders[0].dim.radius *= thisx->scale.x;
    thisv->colliders[0].dim.height = thisx->scale.y * 12000.0f;
    thisv->colliders[0].dim.yShift = 0;
    thisv->colliders[1].dim.radius *= thisx->scale.x;
    thisv->colliders[1].dim.height *= thisx->scale.y;
    thisv->colliders[1].dim.yShift *= thisx->scale.y;
    thisv->actionFunc = NULL;
    thisx->flags &= ~ACTOR_FLAG_0;
    func_809CADDC(thisv, globalCtx);
}

void EnBrob_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBrob* thisv = (EnBrob*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyCylinder(globalCtx, &thisv->colliders[0]);
    Collider_DestroyCylinder(globalCtx, &thisv->colliders[1]);
}

void func_809CADDC(EnBrob* thisv, GlobalContext* globalCtx) {
    func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    thisv->timer = thisv->actionFunc == func_809CB2B8 ? 200 : 0;
    thisv->unk_1AE = 0;
    thisv->actionFunc = func_809CB054;
}

void func_809CAE44(EnBrob* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &object_brob_Anim_001750);
    func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    thisv->unk_1AE = 1000;
    thisv->actionFunc = func_809CB114;
}

void func_809CAEA0(EnBrob* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_brob_Anim_001958, -5.0f);
    thisv->unk_1AE = 8000;
    thisv->timer = 1200;
    thisv->actionFunc = func_809CB218;
}

void func_809CAEF4(EnBrob* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_brob_Anim_000290, -5.0f);
    thisv->unk_1AE -= 125.0f;
    Actor_SetColorFilter(&thisv->dyna.actor, 0, 0xFF, 0, 0x50);
    Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->actionFunc = func_809CB2B8;
}

void func_809CAF88(EnBrob* thisv) {
    Animation_Change(&thisv->skelAnime, &object_brob_Anim_001750, -1.0f,
                     Animation_GetLastFrame(&object_brob_Anim_001750), 0.0f, ANIMMODE_ONCE, -5.0f);
    thisv->unk_1AE = 8250;
    thisv->actionFunc = func_809CB354;
}

void func_809CB008(EnBrob* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_brob_Anim_001678, -5.0f);
    thisv->timer = 10;
    thisv->actionFunc = func_809CB458;
}

void func_809CB054(EnBrob* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->timer == 0) {
        if (func_8004356C(&thisv->dyna) != 0) {
            func_8002F71C(globalCtx, &thisv->dyna.actor, 5.0f, thisv->dyna.actor.yawTowardsPlayer, 1.0f);
            func_809CAE44(thisv, globalCtx);
        } else if (thisv->dyna.actor.xzDistToPlayer < 300.0f) {
            func_809CAE44(thisv, globalCtx);
        }
    } else if (thisv->timer >= 81) {
        thisv->dyna.actor.colorFilterTimer = 80;
    }
}

void func_809CB114(EnBrob* thisv, GlobalContext* globalCtx) {
    f32 curFrame;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_809CAEA0(thisv);
    } else {
        curFrame = thisv->skelAnime.curFrame;
        if (curFrame < 8.0f) {
            thisv->unk_1AE += 1000.0f;
        } else if (curFrame < 12.0f) {
            thisv->unk_1AE += 250.0f;
        } else {
            thisv->unk_1AE -= 250.0f;
        }
    }
}

void func_809CB218(EnBrob* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 6.0f) || Animation_OnFrame(&thisv->skelAnime, 15.0f)) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_BROB_WAVE);
    }
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if ((thisv->timer == 0) && (thisv->dyna.actor.xzDistToPlayer > 500.0f)) {
        func_809CAF88(thisv);
    }
}

void func_809CB2B8(EnBrob* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_809CADDC(thisv, globalCtx);
    } else if (thisv->skelAnime.curFrame < 8.0f) {
        thisv->unk_1AE -= 1250.0f;
    }
    thisv->dyna.actor.colorFilterTimer = 0x50;
}

void func_809CB354(EnBrob* thisv, GlobalContext* globalCtx) {
    f32 curFrame;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_809CADDC(thisv, globalCtx);
    } else {
        curFrame = thisv->skelAnime.curFrame;
        if (curFrame < 8.0f) {
            thisv->unk_1AE -= 1000.0f;
        } else if (curFrame < 12.0f) {
            thisv->unk_1AE -= 250.0f;
        } else {
            thisv->unk_1AE += 250.0f;
        }
    }
}

void func_809CB458(EnBrob* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    f32 dist1;
    f32 dist2;
    s32 i;

    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0) && (thisv->timer != 0)) {
        thisv->timer--;
    }

    dist1 = globalCtx->gameplayFrames % 2 ? 0.0f : thisv->dyna.actor.scale.x * 5500.0f;
    dist2 = thisv->dyna.actor.scale.x * 5500.0f;

    for (i = 0; i < 4; i++) {
        static Color_RGBA8 primColor = { 255, 255, 255, 255 };
        static Color_RGBA8 envColor = { 200, 255, 255, 255 };

        if (i % 2) {
            pos.x = thisv->dyna.actor.world.pos.x + dist1;
            pos.z = thisv->dyna.actor.world.pos.z + dist2;
        } else {
            pos.x = thisv->dyna.actor.world.pos.x + dist2;
            pos.z = thisv->dyna.actor.world.pos.z + dist1;
            dist1 = -dist1;
            dist2 = -dist2;
        }
        pos.y = (((Rand_ZeroOne() * 15000.0f) + 1000.0f) * thisv->dyna.actor.scale.y) + thisv->dyna.actor.world.pos.y;
        EffectSsLightning_Spawn(globalCtx, &pos, &primColor, &envColor, thisv->dyna.actor.scale.y * 8000.0f,
                                Rand_ZeroOne() * 65536.0f, 4, 1);
    }

    if (thisv->timer == 0) {
        func_809CAEA0(thisv);
    }
}

void EnBrob_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnBrob* thisv = (EnBrob*)thisx;
    s32 i;
    s32 acHits[2];

    acHits[0] = (thisv->colliders[0].base.acFlags & AC_HIT) != 0;
    acHits[1] = (thisv->colliders[1].base.acFlags & AC_HIT) != 0;
    if ((acHits[0] && (thisv->colliders[0].info.acHitInfo->toucher.dmgFlags & 0x10)) ||
        (acHits[1] && (thisv->colliders[1].info.acHitInfo->toucher.dmgFlags & 0x10))) {

        for (i = 0; i < 2; i++) {
            thisv->colliders[i].base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            thisv->colliders[i].base.acFlags &= ~AC_HIT;
        }

        func_809CAEF4(thisv);
    } else if ((thisv->colliders[0].base.atFlags & AT_HIT) || (thisv->colliders[1].base.atFlags & AT_HIT) ||
               (acHits[0] && (thisv->colliders[0].info.acHitInfo->toucher.dmgFlags & 0x100)) ||
               (acHits[1] && (thisv->colliders[1].info.acHitInfo->toucher.dmgFlags & 0x100))) {

        if (thisv->actionFunc == func_809CB114 && !(thisv->colliders[0].base.atFlags & AT_BOUNCED) &&
            !(thisv->colliders[1].base.atFlags & AT_BOUNCED)) {
            func_8002F71C(globalCtx, &thisv->dyna.actor, 5.0f, thisv->dyna.actor.yawTowardsPlayer, 1.0f);
        } else if (thisv->actionFunc != func_809CB114) {
            func_809CB008(thisv);
        }

        for (i = 0; i < 2; i++) {
            thisv->colliders[i].base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            thisv->colliders[i].base.acFlags &= ~AC_HIT;
        }
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actionFunc != func_809CB054 && thisv->actionFunc != func_809CB354) {
        if (thisv->actionFunc != func_809CB2B8) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[0].base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[1].base);
            if (thisv->actionFunc != func_809CB114) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[0].base);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[1].base);
            }
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[0].base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[1].base);
    }
}

void EnBrob_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnBrob* thisv = (EnBrob*)thisx;
    MtxF mtx;

    Matrix_Get(&mtx);
    if (limbIndex == 3) {
        thisv->colliders[0].dim.pos.x = mtx.mf_raw.xw;
        thisv->colliders[0].dim.pos.y = mtx.mf_raw.yw;
        thisv->colliders[0].dim.pos.z = mtx.mf_raw.zw;
    } else if (limbIndex == 8) {
        thisv->colliders[1].dim.pos.x = mtx.mf_raw.xw;
        thisv->colliders[1].dim.pos.y = (mtx.mf_raw.yw + 7.0f);
        thisv->colliders[1].dim.pos.z = mtx.mf_raw.zw;
    }
}

void EnBrob_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBrob* thisv = (EnBrob*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Translate(0.0f, thisv->unk_1AE, 0.0f, MTXMODE_APPLY);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, EnBrob_PostLimbDraw, thisv);
}
