#include "z_bg_jya_cobra.h"

#include <string.h>

#include "overlays/actors/ovl_Bg_Jya_Bigmirror/z_bg_jya_bigmirror.h"
#include "overlays/actors/ovl_Mir_Ray/z_mir_ray.h"
#include "objects/object_jya_obj/object_jya_obj.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaCobra_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaCobra_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaCobra_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaCobra_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80896918(BgJyaCobra* thisv, GlobalContext* globalCtx);
void func_80896950(BgJyaCobra* thisv, GlobalContext* globalCtx);
void func_808969F8(BgJyaCobra* thisv, GlobalContext* globalCtx);
void func_80896ABC(BgJyaCobra* thisv, GlobalContext* globalCtx);

#include "overlays/ovl_Bg_Jya_Cobra/ovl_Bg_Jya_Cobra.h"

ActorInit Bg_Jya_Cobra_InitVars = {
    ACTOR_BG_JYA_COBRA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaCobra),
    (ActorFunc)BgJyaCobra_Init,
    (ActorFunc)BgJyaCobra_Destroy,
    (ActorFunc)BgJyaCobra_Update,
    (ActorFunc)BgJyaCobra_Draw,
    NULL,
};

static s16 D_80897308[] = { 0, 0, 0, 0 };

static u8 D_80897310[] = { true, false, true, false };

static s16 D_80897314[] = { -0x4000, 0000, 0x4000, 0000 };

static u8 D_8089731C[11][11] = {
    { 0x00, 0x00, 0x20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x20, 0x00, 0x00 },
    { 0x00, 0x20, 0x80, 0xA0, 0xA3, 0xA3, 0xA3, 0xA0, 0x80, 0x20, 0x00 },
    { 0x20, 0x80, 0xA0, 0xA5, 0xA6, 0xA6, 0xA6, 0xA5, 0xA0, 0x80, 0x20 },
    { 0x80, 0xA0, 0xA5, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA5, 0xA0, 0x80 },
    { 0x80, 0xA3, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA3, 0x80 },
    { 0x80, 0xA3, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA3, 0x80 },
    { 0x80, 0xA3, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA3, 0x80 },
    { 0x80, 0xA0, 0xA5, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA5, 0xA0, 0x80 },
    { 0x20, 0x80, 0xA0, 0xA5, 0xA6, 0xA6, 0xA6, 0xA5, 0xA0, 0x80, 0x20 },
    { 0x00, 0x20, 0x80, 0xA0, 0xA3, 0xA3, 0xA3, 0xA0, 0x80, 0x20, 0x00 },
    { 0x00, 0x00, 0x20, 0x80, 0x80, 0x80, 0x80, 0x80, 0x20, 0x00, 0x00 },
};

static u8 D_80897398[3][3] = {
    { 0x20, 0x80, 0x20 },
    { 0x80, 0xA0, 0x80 },
    { 0x20, 0x80, 0x20 },
};

static Vec3f D_808973A4[] = {
    { -6.0f, 100.0f, 7.6f },
    { -12.6f, 69.200005f, -10.0f },
    { -9.0f, 43.0f, -1.0f },
    { -3.0f, 15.0f, 8.6f },
    { -8.6f, 15.0f, 13.5f },
    { -6.6f, 26.0f, 11.6f },
    { -12.5f, 43.0f, 8.0f },
    { -17.2f, 70.0f, 0.6f },
    { -8.0f, 100.0f, 7.6f },
    { 6.0f, 100.0f, 7.6f },
    { 12.6f, 69.200005f, -10.0f },
    { 9.0f, 43.0f, -1.0f },
    { 3.0f, 15.0f, 8.6f },
    { 8.6f, 15.0f, 13.5f },
    { 6.6f, 26.0f, 11.6f },
    { 12.5f, 43.0f, 8.0f },
    { 17.2f, 70.0f, 0.6f },
    { 8.0f, 100.0f, 7.6f },
    { 0.0f, 70.0f, -11.3f },
    { 0.0f, 44.600002f, -2.0f },
    { 0.0f, 15.0f, 10.6f },
    { 0.0f, 15.0f, 0.3f },
    { 0.0f, 26.0f, 11.6f },
    { 0.0f, 88.4f, -1.4f },
    { 0.0f, 95.700005f, 14.900001f },
    { 0.0f, 101.4f, 5.0f },
};

static Vec3f D_808974DC[] = {
    { 12.0f, 21.300001f, -2.5f },  { 30.0f, 21.300001f, -2.5f }, { -15.0f, 21.300001f, -2.5f },
    { -30.0f, 21.300001f, -2.5f }, { 12.0f, 21.300001f, -2.5f },
};

static s32 D_80897518[] = { 0x80, 0xA0, 0xA0, 0x80 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static Vec3s D_80897538 = { 0, -0x4000, 0 };

static Vec3s D_80897540 = { 0, 0x4000, 0 };

static Vec3f D_80897548[] = {
    { 0.1f, 0.1f, 0.1f },
    { 0.072f, 0.072f, 0.072f },
    { 0.1f, 0.1f, 0.132f },
};

void func_808958F0(Vec3f* dest, Vec3f* src, f32 arg2, f32 arg3) {
    dest->x = (src->z * arg2) + (src->x * arg3);
    dest->y = src->y;
    dest->z = (src->z * arg3) - (src->x * arg2);
}

void BgJyaCobra_InitDynapoly(BgJyaCobra* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 flags) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG Registration Failure"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_cobra.c", 247,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgJyaCobra_SpawnRay(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_MIR_RAY, thisv->dyna.actor.world.pos.x,
                       thisv->dyna.actor.world.pos.y + 57.0f, thisv->dyna.actor.world.pos.z, 0, 0, 0, 6);
    if (thisv->dyna.actor.child == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        // "Ｅｒｒｏｒ : Mir Ray occurrence failure"
        osSyncPrintf("Ｅｒｒｏｒ : Mir Ray 発生失敗 (%s %d)\n", "../z_bg_jya_cobra.c", 270);
        osSyncPrintf(VT_RST);
    }
}

void func_80895A70(BgJyaCobra* thisv) {
    s32 pad;
    BgJyaBigmirror* mirror = (BgJyaBigmirror*)thisv->dyna.actor.parent;
    MirRay* mirRay;

    switch (thisv->dyna.actor.params & 3) {
        case 0:
            mirRay = (MirRay*)thisv->dyna.actor.child;
            if (mirRay == NULL) {
                return;
            }
            if (thisv->dyna.actor.child->update == NULL) {
                thisv->dyna.actor.child = NULL;
                return;
            }
            break;
        case 1:
            mirRay = (MirRay*)mirror->lightBeams[1];
            if (mirRay == NULL) {
                return;
            }
            break;
        case 2:
            mirRay = (MirRay*)mirror->lightBeams[2];
            if (mirRay == NULL) {
                return;
            }
            break;
    }

    if (thisv->unk_18C <= 0.0f) {
        mirRay->unLit = 1;
    } else {
        Vec3f sp28;

        mirRay->unLit = 0;
        Math_Vec3f_Copy(&mirRay->sourcePt, &thisv->unk_180);
        Matrix_RotateY(thisv->dyna.actor.shape.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_NEW);
        Matrix_RotateX(D_80897308[thisv->dyna.actor.params & 3] * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        sp28.x = 0.0f;
        sp28.y = 0.0;
        sp28.z = thisv->unk_190 * 2800.0f;
        Matrix_MultVec3f(&sp28, &mirRay->poolPt);
        Math_Vec3f_Sum(&mirRay->sourcePt, &mirRay->poolPt, &mirRay->poolPt);
    }
}

void func_80895BEC(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    Vec3f sp2C;

    func_808958F0(&sp2C, &thisv->unk_174, Math_SinS(thisv->unk_170), Math_CosS(thisv->unk_170));
    player->actor.world.pos.x = thisv->dyna.actor.world.pos.x + sp2C.x;
    player->actor.world.pos.y = thisv->dyna.actor.world.pos.y + sp2C.y;
    player->actor.world.pos.z = thisv->dyna.actor.world.pos.z + sp2C.z;
}

void func_80895C74(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    s16 phi_v0;
    s16 params = thisv->dyna.actor.params;
    BgJyaBigmirror* mirror = (BgJyaBigmirror*)thisv->dyna.actor.parent;
    f32 phi_f0;

    if ((params & 3) == 2 && mirror != NULL &&
        (!(mirror->puzzleFlags & BIGMIR_PUZZLE_BOMBIWA_DESTROYED) ||
         !(mirror->puzzleFlags & BIGMIR_PUZZLE_COBRA1_SOLVED))) {
        Math_StepToF(&thisv->unk_18C, 0.0f, 0.05f);
    } else {
        thisv->unk_18C = 1.0f;
        if (D_80897310[params & 3]) {
            phi_v0 = thisv->dyna.actor.shape.rot.y - D_80897314[params & 3];
            phi_v0 = ABS(phi_v0);
            if (phi_v0 < 0x2000 && phi_v0 != -0x8000) {
                thisv->unk_18C += (phi_v0 - 0x2000) * (3.0f / 0x4000);
                if (thisv->unk_18C < 0.0f) {
                    thisv->unk_18C = 0.0f;
                }
            }
        }
    }

    thisv->unk_180.x = thisv->dyna.actor.world.pos.x;
    thisv->unk_180.y = thisv->dyna.actor.world.pos.y + 57.0f;
    thisv->unk_180.z = thisv->dyna.actor.world.pos.z;

    if ((params & 3) == 0) {
        thisv->unk_190 = 0.1f;
    } else if ((params & 3) == 1) {
        phi_f0 = 0.1f;
        phi_v0 = thisv->dyna.actor.shape.rot.y - 0x8000;
        if (phi_v0 < 0x500 && phi_v0 > -0x500) {
            phi_f0 = 0.34f;
        } else {
            phi_v0 = thisv->dyna.actor.shape.rot.y - 0x4000;
            if (phi_v0 < 0x500 && phi_v0 > -0x500 && mirror != NULL &&
                (mirror->puzzleFlags & BIGMIR_PUZZLE_BOMBIWA_DESTROYED)) {
                phi_f0 = 0.34f;
            }
        }
        Math_StepToF(&thisv->unk_190, phi_f0, 0.04f);
    } else if ((params & 3) == 2) {
        phi_f0 = 0.1f;
        phi_v0 = thisv->dyna.actor.shape.rot.y - 0x8000;
        if (phi_v0 < 0x500 && phi_v0 > -0x500) {
            phi_f0 = 0.34f;
        } else {
            phi_v0 = thisv->dyna.actor.shape.rot.y + 0xFFFF4000;
            if (phi_v0 < 0x500 && phi_v0 > -0x500) {
                phi_f0 = 0.34f;
            }
        }
        Math_StepToF(&thisv->unk_190, phi_f0, 0.04f);
    }
}

/*
 * Updates the shadow with light coming from the side of the mirror
 */
void BgJyaCobra_UpdateShadowFromSide(BgJyaCobra* thisv) {
    Vec3f spD4;
    Vec3f spC8;
    Vec3f spBC;
    u8* temp_s2;
    s32 temp_x;
    s32 temp_z;
    s32 x;
    s32 z;
    s32 i;
    s32 j;
    s32 k;
    s32 l;
    s16 rotY;

    temp_s2 = reinterpret_cast<u8*>( ALIGN16((uintptr_t)(&thisv->shadowTexture)) );
    memset(temp_s2, 0, 0x1000);

    Matrix_RotateX((std::numbers::pi_v<float> / 4), MTXMODE_NEW);
    rotY = !(thisv->dyna.actor.params & 3) ? (thisv->dyna.actor.shape.rot.y + 0x4000)
                                          : (thisv->dyna.actor.shape.rot.y - 0x4000);
    Matrix_RotateY(rotY * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(0.9f, 0.9f, 0.9f, MTXMODE_APPLY);

    for (i = 0; i < 25; i++) {
        Math_Vec3f_Diff(&D_808973A4[i + 1], &D_808973A4[i], &spD4);
        spD4.x *= 1 / 2.0f;
        spD4.y *= 1 / 2.0f;
        spD4.z *= 1 / 2.0f;
        for (j = 0; j < 2; j++) {
            spC8.x = D_808973A4[i].x + (spD4.x * j);
            spC8.y = D_808973A4[i].y + (spD4.y * j);
            spC8.z = D_808973A4[i].z + (spD4.z * j);
            Matrix_MultVec3f(&spC8, &spBC);
            x = (spBC.x + 50.0f) * 0.64f + 0.5f;
            z = (88.0f - spBC.z) * 0.64f + 0.5f;
            for (k = 0; k < 11; k++) {
                temp_z = z - 5 + k;
                if (!(temp_z & ~0x3F)) {
                    temp_z *= 0x40;
                    for (l = 0; l < 11; l++) {
                        temp_x = x - 5 + l;
                        if (!(temp_x & ~0x3F)) {
                            temp_s2[temp_z + temp_x] |= D_8089731C[k][l];
                        }
                        if (1) {}
                    }
                }
            }
        }
    }

    for (i = 0; i < 4; i++) {
        Math_Vec3f_Diff(&D_808974DC[i + 1], &D_808974DC[i], &spD4);
        spD4.x *= 1 / 5.0f;
        spD4.y *= 1 / 5.0f;
        spD4.z *= 1 / 5.0f;
        for (j = 0; j < 5; j++) {
            spC8.x = D_808974DC[i].x + (spD4.x * j);
            spC8.y = D_808974DC[i].y + (spD4.y * j);
            spC8.z = D_808974DC[i].z + (spD4.z * j);
            Matrix_MultVec3f(&spC8, &spBC);
            x = (s32)(((spBC.x + 50.0f) * 0.64f) + 0.5f);
            z = (s32)(((88.0f - spBC.z) * 0.64f) + 0.5f);
            for (k = 0; k < 3; k++) {
                temp_z = z - 1 + k;
                if (!(temp_z & ~0x3F)) {
                    temp_z *= 0x40;
                    for (l = 0; l < 3; l++) {
                        temp_x = x - 1 + l;
                        if (!(temp_x & ~0x3F)) {
                            temp_s2[temp_z + temp_x] |= D_80897398[k][l];
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < 0x40; i++) {
        temp_s2[0 * 0x40 + i] = 0;
        temp_s2[0x3F * 0x40 + i] = 0;
    }

    for (j = 1; j < 0x3F; j++) {
        temp_s2[j * 0x40 + 0] = 0;
        temp_s2[j * 0x40 + 0x3F] = 0;
    }
    if (D_80897398[0][0]) {}
}

/*
 * Updates the shadow with light coming from above the mirror
 */
void BgJyaCobra_UpdateShadowFromTop(BgJyaCobra* thisv) {
    f32 sp58[0x40];
    s32 i;
    s32 j;
    s32 i_copy;
    s32 counter;
    u8* temp_s0;
    u8* sp40;

    for (i = 0; i < 0x40; i++) {
        sp58[i] = SQ(i - 31.5f);
    }

    sp40 = temp_s0 = (u8*)ALIGN16((uintptr_t)(&thisv->shadowTexture));
    memset(temp_s0, 0, 0x1000);

    for (i = 0; i != 0x40; i++) {
        f32 temp_f12 = sp58[i];

        for (j = 0; j < 0x40; j++, sp40++) {
            f32 temp_f2 = (sp58[j] * 0.5f) + temp_f12;

            if (temp_f2 < 300.0f) {
                *sp40 |= CLAMP_MAX(640 - (s32)(temp_f2 * 2.0f), 166);
            }
        }
    }

    for (i_copy = 0x780, counter = 0; counter < 4; counter++, i_copy += 0x40) {
        i = i_copy;
        for (j = 4; j < 0x3C; j++) {
            if (temp_s0[i_copy + j] < D_80897518[counter]) {
                temp_s0[i_copy + j] = D_80897518[counter];
            }
        }
        temp_s0[i + 0x3C] = 0x20;
        temp_s0[i + 0x3] = 0x20;
    }
}

void BgJyaCobra_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaCobra* thisv = (BgJyaCobra*)thisx;

    BgJyaCobra_InitDynapoly(thisv, globalCtx, &gCobraCol, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    if (!(thisv->dyna.actor.params & 3) && Flags_GetSwitch(globalCtx, ((s32)thisv->dyna.actor.params >> 8) & 0x3F)) {
        thisv->dyna.actor.world.rot.y = thisv->dyna.actor.home.rot.y = thisv->dyna.actor.shape.rot.y = 0;
    }

    if (!(thisv->dyna.actor.params & 3)) {
        BgJyaCobra_SpawnRay(thisv, globalCtx);
    }

    func_80896918(thisv, globalCtx);

    if ((thisv->dyna.actor.params & 3) == 1 || (thisv->dyna.actor.params & 3) == 2) {
        thisv->dyna.actor.room = -1;
    }

    if ((thisv->dyna.actor.params & 3) == 1) {
        BgJyaCobra_UpdateShadowFromTop(thisv);
    }

    // "(jya cobra)"
    osSyncPrintf("(jya コブラ)(arg_data 0x%04x)(act %x)(txt %x)(txt16 %x)\n", thisv->dyna.actor.params, thisv,
                 &thisv->shadowTexture, ALIGN16((s32)(&thisv->shadowTexture)));
}

void BgJyaCobra_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaCobra* thisv = (BgJyaCobra*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_80896918(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = func_80896950;
    thisv->unk_168 = 0;
    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y =
        (thisv->unk_16C * 0x2000) + thisv->dyna.actor.home.rot.y;
}

void func_80896950(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->dyna.unk_150 > 0.001f) {
        thisv->unk_168++;
        if (thisv->unk_168 >= 15) {
            func_808969F8(thisv, globalCtx);
        }
    } else {
        thisv->unk_168 = 0;
    }

    if (fabsf(thisv->dyna.unk_150) > 0.001f) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
}

void func_808969F8(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 phi_a3;
    s16 temp2;

    thisv->actionFunc = func_80896ABC;

    temp2 = thisv->dyna.actor.yawTowardsPlayer - thisv->dyna.actor.shape.rot.y;
    phi_a3 = (s16)(thisv->dyna.actor.shape.rot.y - thisv->dyna.unk_158);
    phi_a3 = ABS(phi_a3);

    if (temp2 > 0) {
        thisv->unk_16A = (phi_a3 > 0x4000) ? 1 : -1;
    } else {
        thisv->unk_16A = (phi_a3 > 0x4000) ? -1 : 1;
    }

    thisv->unk_174.x = player->actor.world.pos.x - thisv->dyna.actor.world.pos.x;
    thisv->unk_174.y = player->actor.world.pos.y - thisv->dyna.actor.world.pos.y;
    thisv->unk_174.z = player->actor.world.pos.z - thisv->dyna.actor.world.pos.z;
    thisv->unk_170 = thisv->unk_16E = 0;
    thisv->unk_172 = true;
}

void func_80896ABC(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    s16 temp_v0;
    Player* player = GET_PLAYER(globalCtx);

    temp_v0 = (s16)((thisv->unk_16C * 0x2000) + thisv->dyna.actor.home.rot.y) - thisv->dyna.actor.world.rot.y;
    if (ABS(temp_v0) < 7424) {
        Math_StepToS(&thisv->unk_16E, 106, 4);
    } else {
        Math_StepToS(&thisv->unk_16E, 21, 10);
    }

    if (Math_ScaledStepToS(&thisv->unk_170, thisv->unk_16A * 0x2000, thisv->unk_16E)) {
        thisv->unk_16C = (thisv->unk_16C + thisv->unk_16A) & 7;
        player->stateFlags2 &= ~0x10;
        thisv->dyna.unk_150 = 0.0f;
        func_80896918(thisv, globalCtx);
    } else {
        thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y =
            (thisv->unk_16C * 0x2000) + thisv->dyna.actor.home.rot.y + thisv->unk_170;
    }

    if (player->stateFlags2 & 0x10) {
        if (thisv->unk_172) {
            func_80895BEC(thisv, globalCtx);
        }
    } else if (fabsf(thisv->dyna.unk_150) < 0.001f) {
        thisv->unk_172 = false;
    }

    thisv->dyna.unk_150 = 0.0f;
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void BgJyaCobra_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgJyaCobra* thisv = (BgJyaCobra*)thisx;

    thisv->actionFunc(thisv, globalCtx);

    func_80895C74(thisv, globalCtx);
    func_80895A70(thisv);

    if ((thisv->dyna.actor.params & 3) == 0 || (thisv->dyna.actor.params & 3) == 2) {
        BgJyaCobra_UpdateShadowFromSide(thisv);
    }
}

void func_80896CB4(GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 864);

    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 867),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gCobra2DL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 872);
}

void func_80896D78(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3s sp44;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 924);
    func_80093D84(globalCtx->state.gfxCtx);

    sp44.x = D_80897308[thisv->dyna.actor.params & 3] + thisv->dyna.actor.shape.rot.x;
    sp44.y = thisv->dyna.actor.shape.rot.y;
    sp44.z = thisv->dyna.actor.shape.rot.z;
    Matrix_SetTranslateRotateYXZ(thisv->unk_180.x, thisv->unk_180.y, thisv->unk_180.z, &sp44);

    Matrix_Scale(0.1f, 0.1f, thisv->unk_190, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 939),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s32)(thisv->unk_18C * 140.0f));
    gSPDisplayList(POLY_XLU_DISP++, gCobra3DL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 947);
}

void BgJyaCobra_DrawShadow(BgJyaCobra* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 params = thisv->dyna.actor.params & 3;
    Vec3f sp64;
    Vec3s* phi_a3;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 966);

    func_80094044(globalCtx->state.gfxCtx);

    if (params == 0) {
        sp64.x = thisv->dyna.actor.world.pos.x - 50.0f;
        sp64.y = thisv->dyna.actor.world.pos.y;
        sp64.z = thisv->dyna.actor.world.pos.z;
        phi_a3 = &D_80897538;
    } else if (params == 2) {
        sp64.x = thisv->dyna.actor.world.pos.x + 70.0f;
        sp64.y = thisv->dyna.actor.world.pos.y;
        sp64.z = thisv->dyna.actor.world.pos.z;
        phi_a3 = &D_80897540;
    } else { // params == 1
        phi_a3 = &thisv->dyna.actor.shape.rot;
        Math_Vec3f_Copy(&sp64, &thisv->dyna.actor.world.pos);
    }

    Matrix_SetTranslateRotateYXZ(sp64.x, sp64.y, sp64.z, phi_a3);

    Matrix_Scale(D_80897548[params].x, D_80897548[params].y, D_80897548[params].z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 40.0f, MTXMODE_APPLY);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 120);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 994),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gDPLoadTextureBlock(POLY_XLU_DISP++, reinterpret_cast<const void*>(ALIGN16((s32)(&thisv->shadowTexture))), G_IM_FMT_I, G_IM_SIZ_8b, 0x40, 0x40, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPDisplayList(POLY_XLU_DISP++, sShadowDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_cobra.c", 1006);
}

void BgJyaCobra_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaCobra* thisv = (BgJyaCobra*)thisx;

    func_80896CB4(globalCtx);
    Gfx_DrawDListOpa(globalCtx, gCobra1DL);

    if (thisv->unk_18C > 0.0f) {
        func_80896D78(thisv, globalCtx);
    }

    if ((thisv->dyna.actor.params & 3) == 2) {
        BgJyaBigmirror* mirror = (BgJyaBigmirror*)thisv->dyna.actor.parent;

        if (mirror != NULL && (mirror->puzzleFlags & BIGMIR_PUZZLE_BOMBIWA_DESTROYED) &&
            (mirror->puzzleFlags & BIGMIR_PUZZLE_COBRA1_SOLVED)) {
            BgJyaCobra_DrawShadow(thisv, globalCtx);
        }
    } else {
        BgJyaCobra_DrawShadow(thisv, globalCtx);
    }
}
