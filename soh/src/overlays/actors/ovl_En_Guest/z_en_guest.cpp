/*
 * File: z_en_guest.c
 * Overlay: ovl_En_Guest
 * Description: Happy Mask Shop Customer
 */

#include "z_en_guest.h"
#include "objects/object_os_anime/object_os_anime.h"
#include "objects/object_boj/object_boj.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnGuest_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGuest_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGuest_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGuest_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A50518(EnGuest* thisv, GlobalContext* globalCtx);
void func_80A5057C(EnGuest* thisv, GlobalContext* globalCtx);
void func_80A505CC(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Guest_InitVars = {
    ACTOR_EN_GUEST,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_BOJ,
    sizeof(EnGuest),
    (ActorFunc)EnGuest_Init,
    (ActorFunc)EnGuest_Destroy,
    (ActorFunc)EnGuest_Update,
    NULL,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 10, 60, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 6, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

void EnGuest_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnGuest* thisv = (EnGuest*)thisx;

    if (gSaveContext.infTable[7] & 0x40) {
        Actor_Kill(&thisv->actor);
    } else {
        thisv->osAnimeBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_OS_ANIME);
        if (thisv->osAnimeBankIndex < 0) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "No such bank!!"
            osSyncPrintf("%s[%d] : バンクが無いよ！！\n", "../z_en_guest.c", 129);
            osSyncPrintf(VT_RST);
            ASSERT(0, "0", "../z_en_guest.c", 132);
        }
    }
}

void EnGuest_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGuest* thisv = (EnGuest*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnGuest_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGuest* thisv = (EnGuest*)thisx;
    s32 pad;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->osAnimeBankIndex)) {
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        Actor_ProcessInitChain(&thisv->actor, sInitChain);

        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_boj_Skel_0000F0, NULL, thisv->jointTable, thisv->morphTable, 16);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->osAnimeBankIndex].segment);
        Animation_Change(&thisv->skelAnime, &gObjOsAnim_42AC, 1.0f, 0.0f, Animation_GetLastFrame(&gObjOsAnim_42AC),
                         ANIMMODE_LOOP, 0.0f);

        thisv->actor.draw = EnGuest_Draw;
        thisv->actor.update = func_80A505CC;

        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);

        Actor_SetFocus(&thisv->actor, 60.0f);

        thisv->unk_30E = 0;
        thisv->unk_30D = 0;
        thisv->unk_2CA = 0;
        thisv->actor.textId = 0x700D;
        thisv->actionFunc = func_80A50518;
    }
}

void func_80A5046C(EnGuest* thisv) {
    if (thisv->unk_30D == 0) {
        if (thisv->unk_2CA != 0) {
            thisv->unk_2CA--;
        } else {
            thisv->unk_30D = 1;
        }
    } else {
        if (thisv->unk_2CA != 0) {
            thisv->unk_2CA--;
        } else {
            thisv->unk_30E += 1;
            if (thisv->unk_30E >= 3) {
                thisv->unk_30E = 0;
                thisv->unk_30D = 0;
                thisv->unk_2CA = (s32)Rand_ZeroFloat(60.0f) + 20;
            } else {
                thisv->unk_2CA = 1;
            }
        }
    }
}

void func_80A50518(EnGuest* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80A5057C;
    } else if (thisv->actor.xzDistToPlayer < 100.0f) {
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    }
}

void func_80A5057C(EnGuest* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        thisv->actionFunc = func_80A50518;
    }
}

void func_80A505CC(Actor* thisx, GlobalContext* globalCtx) {
    EnGuest* thisv = (EnGuest*)thisx;
    s32 pad;
    Player* player;

    player = GET_PLAYER(globalCtx);
    thisv->unk_2C8++;

    func_80A5046C(thisv);
    thisv->actionFunc(thisv, globalCtx);

    thisv->unk_2A0.unk_18 = player->actor.world.pos;
    if (LINK_IS_ADULT) {
        thisv->unk_2A0.unk_14 = 10.0f;
    } else {
        thisv->unk_2A0.unk_14 = 20.0f;
    }
    func_80034A14(&thisv->actor, &thisv->unk_2A0, 6, 2);

    func_80034F54(globalCtx, thisv->unk_2CC, thisv->unk_2EC, 16);

    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->osAnimeBankIndex].segment);

    SkelAnime_Update(&thisv->skelAnime);
    Actor_SetFocus(&thisv->actor, 60.0f);

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

Gfx* func_80A50708(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* dlist;

    dlist = static_cast<Gfx*>(Graph_Alloc(gfxCtx, 2 * sizeof(Gfx)));
    gDPSetEnvColor(dlist, r, g, b, a);
    gSPEndDisplayList(dlist + 1);

    return dlist;
}

s32 EnGuest_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    EnGuest* thisv = (EnGuest*)thisx;
    Vec3s sp3C;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_guest.c", 352);

    if (limbIndex == 15) {
        *dList = object_boj_DL_0059B0;
        Matrix_Translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp3C = thisv->unk_2A0.unk_08;
        Matrix_RotateX((sp3C.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateZ((sp3C.x / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_Translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 8) {
        sp3C = thisv->unk_2A0.unk_0E;
        Matrix_RotateX((-sp3C.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateZ((sp3C.x / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
    }

    if (limbIndex == 8 || limbIndex == 9 || limbIndex == 12) {
        rot->y += Math_SinS(thisv->unk_2CC[limbIndex]) * 200.0f;
        rot->z += Math_CosS(thisv->unk_2EC[limbIndex]) * 200.0f;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_guest.c", 388);

    return false;
}

void EnGuest_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* D_80A50BA4[] = {
        object_boj_Tex_0005FC,
        object_boj_Tex_0006FC,
        object_boj_Tex_0007FC,
    };
    EnGuest* thisv = (EnGuest*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_guest.c", 404);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, func_80A50708(globalCtx->state.gfxCtx, 255, 255, 255, 255));
    gSPSegment(POLY_OPA_DISP++, 0x09, func_80A50708(globalCtx->state.gfxCtx, 160, 60, 220, 255));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(D_80A50BA4[thisv->unk_30E]));

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnGuest_OverrideLimbDraw, NULL, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_guest.c", 421);
}
