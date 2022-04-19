/*
 * File: z_bg_bdan_switch.c
 * Overlay: ovl_Bg_Bdan_Switch
 * Description: Switches Inside Lord Jabu-Jabu
 */

#include "z_bg_bdan_switch.h"
#include "objects/object_bdan_objects/object_bdan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgBdanSwitch_Init(Actor* thisx, GlobalContext* globalCtx);
void BgBdanSwitch_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgBdanSwitch_Update(Actor* thisx, GlobalContext* globalCtx);
void BgBdanSwitch_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_8086D5C4(BgBdanSwitch* thisv);
void func_8086D5E0(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086D67C(BgBdanSwitch* thisv);
void func_8086D694(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086D730(BgBdanSwitch* thisv);
void func_8086D754(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086D7FC(BgBdanSwitch* thisv);
void func_8086D80C(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086D86C(BgBdanSwitch* thisv);
void func_8086D888(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086D8BC(BgBdanSwitch* thisv);
void func_8086D8CC(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086D95C(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086D9F8(BgBdanSwitch* thisv);
void func_8086DA1C(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086DAB4(BgBdanSwitch* thisv);
void func_8086DAC4(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086DB24(BgBdanSwitch* thisv);
void func_8086DB40(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086DB4C(BgBdanSwitch* thisv);
void func_8086DB68(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086DC30(BgBdanSwitch* thisv);
void func_8086DC48(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086DCCC(BgBdanSwitch* thisv);
void func_8086DCE8(BgBdanSwitch* thisv, GlobalContext* globalCtx);
void func_8086DDA8(BgBdanSwitch* thisv);
void func_8086DDC0(BgBdanSwitch* thisv, GlobalContext* globalCtx);

ActorInit Bg_Bdan_Switch_InitVars = {
    ACTOR_BG_BDAN_SWITCH,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_BDAN_OBJECTS,
    sizeof(BgBdanSwitch),
    (ActorFunc)BgBdanSwitch_Init,
    (ActorFunc)BgBdanSwitch_Destroy,
    (ActorFunc)BgBdanSwitch_Update,
    (ActorFunc)BgBdanSwitch_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xEFC1FFFE, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 120, 0 }, 370 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

static Vec3f D_8086E0E0 = { 0.0f, 140.0f, 0.0f };

void BgBdanSwitch_InitDynaPoly(BgBdanSwitch* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 flag) {
    s16 pad1;
    const CollisionHeader* colHeader = NULL;
    s16 pad2;

    DynaPolyActor_Init(&thisv->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_bdan_switch.c", 325,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgBdanSwitch_InitCollision(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    Actor* actor = &thisv->dyna.actor;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, actor, &sJntSphInit, thisv->colliderItems);
}

void func_8086D0EC(BgBdanSwitch* thisv) {
    if (thisv->unk_1CC > 0) {
        thisv->unk_1CC += 0x5DC;
    } else {
        thisv->unk_1CC += 0xFA0;
    }

    switch (thisv->dyna.actor.params & 0xFF) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            thisv->unk_1D4 = ((Math_CosS(thisv->unk_1CC) * 0.5f) + (53.000004f / 6.0f)) * 0.012f;
            thisv->unk_1D0 = ((Math_CosS(thisv->unk_1CC) * 0.5f) + 20.5f) * (thisv->unk_1C8 * 0.0050000004f);
            thisv->dyna.actor.scale.y = thisv->unk_1C8 * 0.1f;
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            thisv->unk_1D4 = ((Math_CosS(thisv->unk_1CC) * 0.5f) + (43.0f / 6.0f)) * 0.0075000003f;
            thisv->unk_1D0 = ((Math_CosS(thisv->unk_1CC) * 0.5f) + 20.5f) * (thisv->unk_1C8 * 0.0050000004f);
            thisv->dyna.actor.scale.y = thisv->unk_1C8 * 0.1f;
    }
    thisv->dyna.actor.shape.yOffset = 1.2f / thisv->unk_1D0;
}

void BgBdanSwitch_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgBdanSwitch* thisv = (BgBdanSwitch*)thisx;
    s32 pad;
    s16 type;
    s32 flag;

    type = thisv->dyna.actor.params & 0xFF;
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    if (type == YELLOW_TALL_1 || type == YELLOW_TALL_2) {
        thisv->dyna.actor.scale.z = 0.05f;
        thisv->dyna.actor.scale.x = 0.05f;
    } else {
        thisv->dyna.actor.scale.z = 0.1f;
        thisv->dyna.actor.scale.x = 0.1f;
    }
    thisv->dyna.actor.scale.y = 0.0f;
    Actor_SetFocus(&thisv->dyna.actor, 10.0f);

    switch (type) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            BgBdanSwitch_InitDynaPoly(thisv, globalCtx, &gJabuFloorSwitchCol, DPM_PLAYER);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            BgBdanSwitch_InitCollision(thisv, globalCtx);
            thisv->dyna.actor.flags |= ACTOR_FLAG_0;
            thisv->dyna.actor.targetMode = 4;
            break;
    }

    flag = Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);

    switch (type) {
        case BLUE:
        case YELLOW:
            if (flag) {
                func_8086D730(thisv);
            } else {
                func_8086D5C4(thisv);
            }
            break;
        case YELLOW_HEAVY:
            if (flag) {
                func_8086DB24(thisv);
            } else {
                func_8086D86C(thisv);
            }
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            if (flag) {
                func_8086DCCC(thisv);
            } else {
                func_8086DB4C(thisv);
            }
            break;
        default:
            osSyncPrintf("不正な ARG_DATA(arg_data 0x%04x)(%s %d)\n", thisv->dyna.actor.params, "../z_bg_bdan_switch.c",
                         454);
            Actor_Kill(&thisv->dyna.actor);
            return;
    }
    osSyncPrintf("(巨大魚ダンジョン 専用スイッチ)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void BgBdanSwitch_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgBdanSwitch* thisv = (BgBdanSwitch*)thisx;

    switch (thisv->dyna.actor.params & 0xFF) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            Collider_DestroyJntSph(globalCtx, &thisv->collider);
            break;
    }
}

void func_8086D4B4(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 type;

    if (!Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
        type = thisv->dyna.actor.params & 0xFF;
        Flags_SetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
        if (type == BLUE || type == YELLOW_TALL_2) {
            OnePointCutscene_AttentionSetSfx(globalCtx, &thisv->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            OnePointCutscene_AttentionSetSfx(globalCtx, &thisv->dyna.actor, NA_SE_SY_CORRECT_CHIME);
        }
    }
}

void func_8086D548(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
        Flags_UnsetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
        if ((thisv->dyna.actor.params & 0xFF) == YELLOW_TALL_2) {
            OnePointCutscene_AttentionSetSfx(globalCtx, &thisv->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
}

void func_8086D5C4(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086D5E0;
    thisv->unk_1C8 = 1.0f;
}

void func_8086D5E0(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    switch (thisv->dyna.actor.params & 0xFF) {
        case BLUE:
            if (func_800435B4(&thisv->dyna)) {
                func_8086D67C(thisv);
                func_8086D4B4(thisv, globalCtx);
            }
            break;
        case YELLOW:
            if (func_8004356C(&thisv->dyna)) {
                func_8086D67C(thisv);
                func_8086D4B4(thisv, globalCtx);
            }
            break;
    }
}

void func_8086D67C(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086D694;
    thisv->unk_1DA = 0x64;
}

void func_8086D694(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    if ((func_8005B198() == thisv->dyna.actor.category) || (thisv->unk_1DA <= 0)) {
        thisv->unk_1C8 -= 0.2f;
        if (thisv->unk_1C8 <= 0.1f) {
            func_8086D730(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
        }
    }
}

void func_8086D730(BgBdanSwitch* thisv) {
    thisv->unk_1C8 = 0.1f;
    thisv->actionFunc = func_8086D754;
    thisv->unk_1D8 = 6;
}

void func_8086D754(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    switch (thisv->dyna.actor.params & 0xFF) {
        case BLUE:
            if (!func_800435B4(&thisv->dyna)) {
                if (thisv->unk_1D8 <= 0) {
                    func_8086D7FC(thisv);
                    func_8086D548(thisv, globalCtx);
                }
            } else {
                thisv->unk_1D8 = 6;
            }
            break;
        case YELLOW:
            if (!Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
                func_8086D7FC(thisv);
            }
            break;
    }
}

void func_8086D7FC(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086D80C;
}

void func_8086D80C(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    thisv->unk_1C8 += 0.2f;
    if (thisv->unk_1C8 >= 1.0f) {
        func_8086D5C4(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void func_8086D86C(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086D888;
    thisv->unk_1C8 = 1.0f;
}

void func_8086D888(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    if (func_8004356C(&thisv->dyna)) {
        func_8086D8BC(thisv);
    }
}

void func_8086D8BC(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086D8CC;
}

void func_8086D8CC(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    thisv->unk_1C8 -= 0.2f;
    if (thisv->unk_1C8 <= 0.6f) {
        func_8086D9F8(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
    }
}

void func_8086D944(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086D95C;
    thisv->unk_1DA = 0x64;
}

void func_8086D95C(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    if ((func_8005B198() == thisv->dyna.actor.category) || (thisv->unk_1DA <= 0)) {
        thisv->unk_1C8 -= 0.2f;
        if (thisv->unk_1C8 <= 0.1f) {
            func_8086DB24(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
        }
    }
}

void func_8086D9F8(BgBdanSwitch* thisv) {
    thisv->unk_1C8 = 0.6f;
    thisv->actionFunc = func_8086DA1C;
    thisv->unk_1D8 = 6;
}

void func_8086DA1C(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    Actor* heldActor = GET_PLAYER(globalCtx)->heldActor;

    if (func_8004356C(&thisv->dyna)) {
        if (heldActor != NULL && heldActor->id == ACTOR_EN_RU1) {
            if (thisv->unk_1D8 <= 0) {
                func_8086D944(thisv);
                func_8086D4B4(thisv, globalCtx);
            }
        } else {
            thisv->unk_1D8 = 6;
        }
    } else {
        if (thisv->unk_1D8 <= 0) {
            func_8086DAB4(thisv);
        }
    }
}

void func_8086DAB4(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086DAC4;
}

void func_8086DAC4(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    thisv->unk_1C8 += 0.2f;
    if (thisv->unk_1C8 >= 1.0f) {
        func_8086D86C(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void func_8086DB24(BgBdanSwitch* thisv) {
    thisv->unk_1C8 = 0.1f;
    thisv->actionFunc = func_8086DB40;
}

void func_8086DB40(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
}

void func_8086DB4C(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086DB68;
    thisv->unk_1C8 = 2.0f;
}

void func_8086DB68(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    switch (thisv->dyna.actor.params & 0xFF) {
        default:
            return;
        case YELLOW_TALL_1:
            if (((thisv->collider.base.acFlags & AC_HIT) != 0) && thisv->unk_1D8 <= 0) {
                thisv->unk_1D8 = 0xA;
                func_8086DC30(thisv);
                func_8086D4B4(thisv, globalCtx);
            }
            break;
        case YELLOW_TALL_2:
            if (((thisv->collider.base.acFlags & AC_HIT) != 0) && ((thisv->unk_1DC & 2) == 0) && thisv->unk_1D8 <= 0) {
                thisv->unk_1D8 = 0xA;
                func_8086DC30(thisv);
                func_8086D4B4(thisv, globalCtx);
            }
            break;
    }
}

void func_8086DC30(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086DC48;
    thisv->unk_1DA = 0x64;
}

void func_8086DC48(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    if ((func_8005B198() == thisv->dyna.actor.category) || (thisv->unk_1DA <= 0)) {
        thisv->unk_1C8 -= 0.3f;
        if (thisv->unk_1C8 <= 1.0f) {
            func_8086DCCC(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void func_8086DCCC(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086DCE8;
    thisv->unk_1C8 = 1.0f;
}

void func_8086DCE8(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    switch (thisv->dyna.actor.params & 0xFF) {
        case YELLOW_TALL_1:
            if (!Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
                func_8086DDA8(thisv);
            }
            break;
        case YELLOW_TALL_2:
            if (((thisv->collider.base.acFlags & AC_HIT) != 0) && ((thisv->unk_1DC & 2) == 0) && (thisv->unk_1D8 <= 0)) {
                thisv->unk_1D8 = 0xA;
                func_8086DDA8(thisv);
                func_8086D548(thisv, globalCtx);
            }
            break;
    }
}

void func_8086DDA8(BgBdanSwitch* thisv) {
    thisv->actionFunc = func_8086DDC0;
    thisv->unk_1DA = 0x64;
}

void func_8086DDC0(BgBdanSwitch* thisv, GlobalContext* globalCtx) {
    if ((((thisv->dyna.actor.params & 0xFF) != YELLOW_TALL_2) || (func_8005B198() == thisv->dyna.actor.category)) ||
        (thisv->unk_1DA <= 0)) {
        thisv->unk_1C8 += 0.3f;
        if (thisv->unk_1C8 >= 2.0f) {
            func_8086DB4C(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void BgBdanSwitch_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgBdanSwitch* thisv = (BgBdanSwitch*)thisx;
    s32 type;

    if (thisv->unk_1DA > 0) {
        thisv->unk_1DA--;
    }
    thisv->actionFunc(thisv, globalCtx);
    func_8086D0EC(thisv);
    type = thisv->dyna.actor.params & 0xFF;
    if (type != 3 && type != 4) {
        thisv->unk_1D8--;
    } else {
        if (!Player_InCsMode(globalCtx) && thisv->unk_1D8 > 0) {
            thisv->unk_1D8--;
        }
        thisv->unk_1DC = thisv->collider.base.acFlags;
        thisv->collider.base.acFlags &= ~AC_HIT;
        thisv->collider.elements[0].dim.modelSphere.radius = thisv->unk_1D4 * 370.0f;
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void func_8086DF58(BgBdanSwitch* thisv, GlobalContext* globalCtx, const Gfx* dlist) {
    Matrix_SetTranslateRotateYXZ(thisv->dyna.actor.world.pos.x,
                                 thisv->dyna.actor.world.pos.y + (thisv->dyna.actor.shape.yOffset * thisv->unk_1D0),
                                 thisv->dyna.actor.world.pos.z, &thisv->dyna.actor.shape.rot);
    Matrix_Scale(thisv->unk_1D4, thisv->unk_1D0, thisv->unk_1D4, MTXMODE_APPLY);
    Gfx_DrawDListOpa(globalCtx, dlist);
}

void BgBdanSwitch_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgBdanSwitch* thisv = (BgBdanSwitch*)thisx;

    switch (thisv->dyna.actor.params & 0xFF) {
        case YELLOW_HEAVY:
        case YELLOW:
            func_8086DF58(thisv, globalCtx, gJabuYellowFloorSwitchDL);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            func_8086DF58(thisv, globalCtx, gJabuYellowFloorSwitchDL);
            Collider_UpdateSpheres(0, &thisv->collider);
            Matrix_MultVec3f(&D_8086E0E0, &thisv->dyna.actor.focus.pos);
            break;
        case BLUE:
            func_8086DF58(thisv, globalCtx, gJabuBlueFloorSwitchDL);
            break;
    }
}
