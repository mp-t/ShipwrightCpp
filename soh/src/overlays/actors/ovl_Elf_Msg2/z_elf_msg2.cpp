/*
 * File: z_elf_msg2.c
 * Overlay: ovl_Elf_Msg2
 * Description: Targetable Navi check spot
 */

#include "z_elf_msg2.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void ElfMsg2_Init(Actor* thisx, GlobalContext* globalCtx);
void ElfMsg2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ElfMsg2_Update(Actor* thisx, GlobalContext* globalCtx);
void ElfMsg2_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 ElfMsg2_GetMessageId(ElfMsg2* thisv);
void ElfMsg2_WaitUntilActivated(ElfMsg2* thisv, GlobalContext* globalCtx);
void ElfMsg2_WaitForTextRead(ElfMsg2* thisv, GlobalContext* globalCtx);

ActorInit Elf_Msg2_InitVars = {
    ACTOR_ELF_MSG2,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ElfMsg2),
    (ActorFunc)ElfMsg2_Init,
    (ActorFunc)ElfMsg2_Destroy,
    (ActorFunc)ElfMsg2_Update,
    (ActorFunc)ElfMsg2_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_STOP),
};

void ElfMsg2_SetupAction(ElfMsg2* thisv, ElfMsg2ActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

/**
 * Checks a scene flag - if flag is set, the actor is killed and function returns 1. Otherwise returns 0.
 * Can also set a switch flag from params while killing.
 */
s32 ElfMsg2_KillCheck(ElfMsg2* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.world.rot.y > 0) && (thisv->actor.world.rot.y < 0x41) &&
        Flags_GetSwitch(globalCtx, thisv->actor.world.rot.y - 1)) {
        LOG_STRING("共倒れ", "../z_elf_msg2.c", 171); // "Mutual destruction"
        if (((thisv->actor.params >> 8) & 0x3F) != 0x3F) {
            Flags_SetSwitch(globalCtx, ((thisv->actor.params >> 8) & 0x3F));
        }
        Actor_Kill(&thisv->actor);
        return 1;
    } else if ((thisv->actor.world.rot.y == -1) && Flags_GetClear(globalCtx, thisv->actor.room)) {
        LOG_STRING("共倒れ２", "../z_elf_msg2.c", 182); // "Mutual destruction 2"
        if (((thisv->actor.params >> 8) & 0x3F) != 0x3F) {
            Flags_SetSwitch(globalCtx, ((thisv->actor.params >> 8) & 0x3F));
        }
        Actor_Kill(&thisv->actor);
        return 1;
    } else if (((thisv->actor.params >> 8) & 0x3F) == 0x3F) {
        return 0;
    } else if (Flags_GetSwitch(globalCtx, ((thisv->actor.params >> 8) & 0x3F))) {
        LOG_STRING("共倒れ", "../z_elf_msg2.c", 192); // "Mutual destruction"
        Actor_Kill(&thisv->actor);
        return 1;
    }
    return 0;
}

void ElfMsg2_Init(Actor* thisx, GlobalContext* globalCtx) {
    ElfMsg2* thisv = (ElfMsg2*)thisx;

    osSyncPrintf(VT_FGCOL(CYAN) " Elf_Msg2_Actor_ct %04x\n\n" VT_RST, thisv->actor.params);
    if (!ElfMsg2_KillCheck(thisv, globalCtx)) {
        if ((thisv->actor.world.rot.x > 0) && (thisv->actor.world.rot.x < 8)) {
            thisv->actor.targetMode = thisv->actor.world.rot.x - 1;
        }
        Actor_ProcessInitChain(thisx, sInitChain);
        if (thisv->actor.world.rot.y >= 0x41) {
            ElfMsg2_SetupAction(thisv, ElfMsg2_WaitUntilActivated);
        } else {
            ElfMsg2_SetupAction(thisv, ElfMsg2_WaitForTextRead);
            thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_18; // Make actor targetable and Navi-checkable
            thisv->actor.textId = ElfMsg2_GetMessageId(thisv);
        }
        thisv->actor.shape.rot.x = thisv->actor.shape.rot.y = thisv->actor.shape.rot.z = 0;
    }
}

void ElfMsg2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

s32 ElfMsg2_GetMessageId(ElfMsg2* thisv) {
    return (thisv->actor.params & 0xFF) + 0x100;
}

/**
 * Runs while Navi text is up. Kills the actor upon closing the text box unless rot.z == 1, can also set a switch flag
 * from params.
 */
void ElfMsg2_WaitForTextClose(ElfMsg2* thisv, GlobalContext* globalCtx) {
    s32 switchFlag;

    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        if (thisv->actor.world.rot.z != 1) {
            Actor_Kill(&thisv->actor);
            switchFlag = (thisv->actor.params >> 8) & 0x3F;
            if (switchFlag != 0x3F) {
                Flags_SetSwitch(globalCtx, switchFlag);
            }
        } else {
            ElfMsg2_SetupAction(thisv, ElfMsg2_WaitForTextRead);
        }
    }
}

/**
 * Runs while Navi text is not up.
 */
void ElfMsg2_WaitForTextRead(ElfMsg2* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        ElfMsg2_SetupAction(thisv, ElfMsg2_WaitForTextClose);
    }
}

/**
 * Idles until a switch flag is set, at which point the actor becomes interactable
 */
void ElfMsg2_WaitUntilActivated(ElfMsg2* thisv, GlobalContext* globalCtx) {
    // If (y >= 0x41) && (y <= 0x80), Idles until switch flag (actor.world.rot.y - 0x41) is set
    // If (y > 0x80), Idles forever (Bug?)
    if ((thisv->actor.world.rot.y >= 0x41) && (thisv->actor.world.rot.y <= 0x80) &&
        (Flags_GetSwitch(globalCtx, (thisv->actor.world.rot.y - 0x41)))) {
        ElfMsg2_SetupAction(thisv, ElfMsg2_WaitForTextRead);
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_18; // Make actor targetable and Navi-checkable
        thisv->actor.textId = ElfMsg2_GetMessageId(thisv);
    }
}

void ElfMsg2_Update(Actor* thisx, GlobalContext* globalCtx) {
    ElfMsg2* thisv = (ElfMsg2*)thisx;

    if (!ElfMsg2_KillCheck(thisv, globalCtx)) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

#include "overlays/ovl_Elf_Msg2/ovl_Elf_Msg2.h"

void ElfMsg2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_elf_msg2.c", 355);

    if (R_NAVI_MSG_REGION_ALPHA == 0) {
        return;
    }

    func_80093D18(globalCtx->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 100, 255, R_NAVI_MSG_REGION_ALPHA);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_elf_msg2.c", 362),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, D_809ADC38);
    gSPDisplayList(POLY_XLU_DISP++, sCubeDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_elf_msg2.c", 367);
}
