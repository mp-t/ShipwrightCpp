#include "z_bg_jya_megami.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_jya_obj/object_jya_obj.h"

#define FLAGS 0

void BgJyaMegami_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaMegami_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaMegami_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaMegami_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgJyaMegami_SetupDetectLight(BgJyaMegami* thisv);
void BgJyaMegami_DetectLight(BgJyaMegami* thisv, GlobalContext* globalCtx);
void BgJyaMegami_SetupExplode(BgJyaMegami* thisv);
void BgJyaMegami_Explode(BgJyaMegami* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Jya_Megami_InitVars = {
    ACTOR_BG_JYA_MEGAMI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaMegami),
    (ActorFunc)BgJyaMegami_Init,
    (ActorFunc)BgJyaMegami_Destroy,
    (ActorFunc)BgJyaMegami_Update,
    (ActorFunc)BgJyaMegami_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00200000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, -600, -200 }, 60 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ f32 velX;
    /* 0x10 */ s16 rotVelX;
    /* 0x12 */ s16 rotVelY;
    /* 0x14 */ s16 delay;
} BgJyaMegamiPieceInit; // size = 0x18

static BgJyaMegamiPieceInit sPiecesInit[] = {
    { { -50.0f, -21.28f, -38.92f }, -1.6f, 0xFED4, 0xFE70, 40 },
    { { -4.32f, -13.44f, -13.76f }, 0.0f, 0x04B0, 0x0190, 42 },
    { { 30.92f, -20.480001f, -28.84f }, 1.6f, 0xFCE0, 0x0320, 44 },
    { { -24.08f, -40.960003f, -21.359999f }, -1.0f, 0xFD44, 0x01F4, 36 },
    { { -44.8f, -73.92f, -49.76f }, -3.2f, 0x03E8, 0xFF38, 30 },
    { { -9.0f, -76.479996f, -13.24f }, -1.0f, 0xFC18, 0x0258, 26 },
    { { -10.240001f, -52.56f, -13.400001f }, 1.6f, 0x0258, 0xFE70, 34 },
    { { 34.04f, -61.72f, -37.04f }, 1.5f, 0x0258, 0x0258, 28 },
    { { 59.48f, -38.399998f, -49.4f }, 1.8f, 0x01F4, 0x0000, 38 },
    { { -19.04f, -112.24f, -35.120003f }, -1.6f, 0x012C, 0x0320, 22 },
    { { 12.24f, -99.04f, -31.64f }, 1.8f, 0xFC18, 0xFE70, 24 },
    { { 49.16f, -81.24f, -55.52f }, 2.4f, 0x02BC, 0x012C, 32 },
    { { 14.759999f, -125.8f, -44.16f }, 0.2f, 0x0320, 0x0258, 20 },
};

static s16 D_8089B14C[] = {
    0x0005, 0x0008, 0x000B, 0x000E, 0x0011, 0x0014, 0x0017, 0x001A,
};

static s16 D_8089B15C[] = {
    0x0012, 0x001A, 0x0022, 0x002A, 0x0032, 0x003C, 0x0046, 0x0050,
};

static s16 D_8089B16C[] = {
    0x0030, 0x002A, 0x0024, 0x0020, 0x001C, 0x0018, 0x0014, 0x0010,
};

static s16 D_8089B17C[] = {
    0x0001,
    0x0003,
    0x0007,
};

static Vec3f sVelocity = { 0.0f, 0.0f, 0.8f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

void BgJyaMegami_InitDynaPoly(BgJyaMegami* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 flag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
}

void BgJyaMegami_InitCollider(BgJyaMegami* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->dyna.actor, &sJntSphInit, &thisv->colliderItem);
}

void BgJyaMegami_SpawnEffect(GlobalContext* globalCtx, Vec3f* pos, Vec3f* velocity, s32 num, s32 arg4) {
    Vec3f spB4;
    s32 i;

    for (i = 0; i < num; i++) {
        s32 idx = ((s16)(Rand_ZeroOne() * 8.0f)) & D_8089B17C[arg4];
        s16 arg5 = ((idx < 5) && (Rand_ZeroOne() < 0.7f)) ? 0x40 : 0x20;
        EffectSsKakera_Spawn(globalCtx, pos, velocity, pos, -90, arg5, D_8089B16C[idx], 4, 0, D_8089B14C[idx], 0, 5,
                             D_8089B15C[idx], KAKERA_COLOR_NONE, OBJECT_JYA_OBJ, gMegami2DL);
        if (Rand_ZeroOne() < 0.45f) {
            Math_Vec3f_Copy(&spB4, pos);
            spB4.z += 25.0f;
            func_80033480(globalCtx, &spB4, 60.0f, 0, D_8089B14C[idx] * 4 + 50, D_8089B14C[idx] * 4 + 70, 1);
        }
    }
}

void BgJyaMegami_SetupSpawnEffect(BgJyaMegami* thisv, GlobalContext* globalCtx, f32 arg2) {
    s32 i;
    Vec3f pos;

    for (i = 0; i < ARRAY_COUNT(thisv->pieces); i++) {
        if (Rand_ZeroOne() < arg2) {
            Math_Vec3f_Sum(&thisv->dyna.actor.world.pos, &sPiecesInit[i].unk_00, &pos);
            pos.z += 15.0f;
            BgJyaMegami_SpawnEffect(globalCtx, &pos, &sVelocity, 1, 0);
        }
    }
}

void BgJyaMegami_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaMegami* thisv = (BgJyaMegami*)thisx;

    BgJyaMegami_InitDynaPoly(thisv, globalCtx, &GMegamiCol, DPM_UNK);
    BgJyaMegami_InitCollider(thisv, globalCtx);
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        Actor_Kill(&thisv->dyna.actor);
    } else {
        Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
        Actor_SetFocus(&thisv->dyna.actor, -50.0f);
        BgJyaMegami_SetupDetectLight(thisv);
    }
}

void BgJyaMegami_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaMegami* thisv = (BgJyaMegami*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void BgJyaMegami_SetupDetectLight(BgJyaMegami* thisv) {
    thisv->actionFunc = BgJyaMegami_DetectLight;
    thisv->lightTimer = 0;
    thisv->crumbleIndex = 0;
}

void BgJyaMegami_DetectLight(BgJyaMegami* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->lightTimer++;
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (globalCtx->gameplayFrames % 4 == 0) {
            BgJyaMegami_SetupSpawnEffect(thisv, globalCtx, (thisv->crumbleIndex * 0.04f) + 0.05f);
        }
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_FACE_CRUMBLE_SLOW - SFX_FLAG);
    } else if (thisv->lightTimer > 0) {
        thisv->lightTimer--;
    }
    if (thisv->lightTimer > 40) {
        Flags_SetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F);
        BgJyaMegami_SetupExplode(thisv);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->dyna.actor.world.pos, 100, NA_SE_EV_FACE_EXPLOSION);
        OnePointCutscene_Init(globalCtx, 3440, -99, &thisv->dyna.actor, MAIN_CAM);
    } else {
        if (thisv->lightTimer < 8) {
            thisv->crumbleIndex = 0;
        } else if (thisv->lightTimer < 16) {
            thisv->crumbleIndex = 1;
        } else if (thisv->lightTimer < 24) {
            thisv->crumbleIndex = 2;
        } else if (thisv->lightTimer < 32) {
            thisv->crumbleIndex = 3;
        } else {
            thisv->crumbleIndex = 4;
        }
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void BgJyaMegami_SetupExplode(BgJyaMegami* thisv) {
    u32 i;

    thisv->actionFunc = BgJyaMegami_Explode;
    for (i = 0; i < ARRAY_COUNT(thisv->pieces); i++) {
        Math_Vec3f_Copy(&thisv->pieces[i].pos, &thisv->dyna.actor.world.pos);
        thisv->pieces[i].vel.x = sPiecesInit[i].velX;
    }
    thisv->explosionTimer = 0;
}

void BgJyaMegami_Explode(BgJyaMegami* thisv, GlobalContext* globalCtx) {
    static Vec3f sVec = { 0.0f, 0.0f, 0.0f };
    BgJyaMegamiPiece* temp;
    u32 i;
    Vec3f sp8C;
    BgJyaMegamiPieceInit* temp2;
    s32 pad;

    thisv->explosionTimer++;
    if (thisv->explosionTimer == 30) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->dyna.actor.world.pos, 100, NA_SE_EV_FACE_BREAKDOWN);
    }

    for (i = 0; i < ARRAY_COUNT(thisv->pieces); i++) {
        temp = &thisv->pieces[i];
        temp2 = &sPiecesInit[i];
        if (thisv->explosionTimer > temp2->delay) {
            temp->vel.y -= 0.6f;
            if (temp->vel.y < -18.0f) {
                temp->vel.y = -18.0f;
            }
            temp->vel.x *= 0.995f;
            temp->pos.x += temp->vel.x;
            temp->pos.y += temp->vel.y;
            temp->rotVelX += temp2->rotVelX;
            temp->rotVelY += temp2->rotVelY;
            if (Rand_ZeroOne() < 0.067f) {
                Math_Vec3f_Sum(&temp->pos, &temp2->unk_00, &sp8C);
                sp8C.z += 10.0f;
                BgJyaMegami_SpawnEffect(globalCtx, &sp8C, &temp->vel, 3, 2);
            }
        } else if (thisv->explosionTimer == temp2->delay) {
            Math_Vec3f_Sum(&temp->pos, &temp2->unk_00, &sp8C);
            sp8C.z += 10.0f;
            BgJyaMegami_SpawnEffect(globalCtx, &sp8C, &temp->vel, 4, 2);
        }
    }

    if ((thisv->explosionTimer % 4 == 0) && (thisv->explosionTimer > 30) && (thisv->explosionTimer < 80) &&
        (thisv->explosionTimer > 40)) {
        sp8C.x = ((Rand_ZeroOne() - 0.5f) * 90.0f) + thisv->dyna.actor.world.pos.x;
        sp8C.y = (thisv->dyna.actor.world.pos.y - (Rand_ZeroOne() * 80.0f)) - 20.0f;
        sp8C.z = thisv->dyna.actor.world.pos.z - (Rand_ZeroOne() - 0.5f) * 50.0f;
        BgJyaMegami_SpawnEffect(globalCtx, &sp8C, &sVec, 1, 0);
    }
    if (thisv->explosionTimer < ARRAY_COUNT(thisv->pieces)) {
        sp8C.x = thisv->dyna.actor.world.pos.x;
        sp8C.y = thisv->dyna.actor.world.pos.y - 60.0f;
        sp8C.z = thisv->dyna.actor.world.pos.z;
        func_80033480(globalCtx, &sp8C, 100.0f, 1, 150, 100, 1);
    }
    if (thisv->explosionTimer == 60) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
    }
    if (thisv->explosionTimer >= 100) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgJyaMegami_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaMegami* thisv = (BgJyaMegami*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

static const void* sRightSideCrumbles[] = {
    gMegamiRightCrumble1Tex, gMegamiRightCrumble2Tex, gMegamiRightCrumble3Tex,
    gMegamiRightCrumble4Tex, gMegamiRightCrumble5Tex,
};

static const void* sLeftSideCrumbles[] = {
    gMegamiLeftCrumble1Tex, gMegamiLeftCrumble2Tex, gMegamiLeftCrumble3Tex,
    gMegamiLeftCrumble4Tex, gMegamiLeftCrumble5Tex,
};

void BgJyaMegami_DrawFace(BgJyaMegami* thisv, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_megami.c", 706);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sRightSideCrumbles[thisv->crumbleIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sLeftSideCrumbles[thisv->crumbleIndex]));
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_megami.c", 716),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gMegami1DL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_megami.c", 720);
}

static const Gfx* sDLists[] = {
    gMegamiPiece1DL,  gMegamiPiece2DL,  gMegamiPiece3DL,  gMegamiPiece4DL, gMegamiPiece5DL,
    gMegamiPiece6DL,  gMegamiPiece7DL,  gMegamiPiece8DL,  gMegamiPiece9DL, gMegamiPiece10DL,
    gMegamiPiece11DL, gMegamiPiece12DL, gMegamiPiece13DL,
};

void BgJyaMegami_DrawExplode(BgJyaMegami* thisv, GlobalContext* globalCtx) {
    s32 pad;
    BgJyaMegamiPiece* piece;
    u32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_megami.c", 746);

    func_80093D18(globalCtx->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(thisv->pieces); i++) {
        piece = &thisv->pieces[i];
        Matrix_Translate(piece->pos.x + sPiecesInit[i].unk_00.x, piece->pos.y + sPiecesInit[i].unk_00.y,
                         piece->pos.z + sPiecesInit[i].unk_00.z, MTXMODE_NEW);
        Matrix_RotateY(piece->rotVelY * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_RotateX(piece->rotVelX * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
        Matrix_Translate(sPiecesInit[i].unk_00.x * -10.0f, sPiecesInit[i].unk_00.y * -10.0f,
                         sPiecesInit[i].unk_00.z * -10.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_megami.c", 778),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, sDLists[i]);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_megami.c", 783);
}

void BgJyaMegami_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaMegami* thisv = (BgJyaMegami*)thisx;

    Collider_UpdateSpheres(0, &thisv->collider);
    if (thisv->actionFunc == BgJyaMegami_Explode) {
        BgJyaMegami_DrawExplode(thisv, globalCtx);
    } else {
        BgJyaMegami_DrawFace(thisv, globalCtx);
    }
}
