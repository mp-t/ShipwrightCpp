/*
 * File: z_obj_timeblock.c
 * Overlay: ovl_Obj_Timeblock
 * Description: Song of Time Block
 */

#include "z_obj_timeblock.h"
#include "objects/object_timeblock/object_timeblock.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4 | ACTOR_FLAG_25 | ACTOR_FLAG_27)

void ObjTimeblock_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjTimeblock_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjTimeblock_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjTimeblock_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjTimeblock_SetupNormal(ObjTimeblock* thisv);
void ObjTimeblock_SetupAltBehaviorVisible(ObjTimeblock* thisv);
void ObjTimeblock_SetupAltBehaviourNotVisible(ObjTimeblock* thisv);

s32 ObjTimeblock_WaitForOcarina(ObjTimeblock* thisv, GlobalContext* globalCtx);
s32 ObjTimeblock_WaitForSong(ObjTimeblock* thisv, GlobalContext* globalCtx);
void ObjTimeblock_DoNothing(ObjTimeblock* thisv, GlobalContext* globalCtx);
void ObjTimeblock_Normal(ObjTimeblock* thisv, GlobalContext* globalCtx);
void ObjTimeblock_AltBehaviorVisible(ObjTimeblock* thisv, GlobalContext* globalCtx);
void ObjTimeblock_AltBehaviourNotVisible(ObjTimeblock* thisv, GlobalContext* globalCtx);

const ActorInit Obj_Timeblock_InitVars = {
    ACTOR_OBJ_TIMEBLOCK,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_TIMEBLOCK,
    sizeof(ObjTimeblock),
    (ActorFunc)ObjTimeblock_Init,
    (ActorFunc)ObjTimeblock_Destroy,
    (ActorFunc)ObjTimeblock_Update,
    (ActorFunc)ObjTimeblock_Draw,
    NULL,
};

typedef struct {
    /* 0x00 */ f32 scale;
    /* 0x04 */ f32 height;
    /* 0x08 */ s16 demoEffectParams;
} ObjTimeblockSizeOptions; // size = 0x0C

static ObjTimeblockSizeOptions sSizeOptions[] = {
    { 1.0, 60.0, 0x0018 },
    { 0.60, 40.0, 0x0019 },
};

static f32 sRanges[] = { 60.0, 100.0, 140.0, 180.0, 220.0, 260.0, 300.0, 300.0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

static Color_RGB8 sPrimColors[] = {
    { 100, 120, 140 }, { 80, 140, 200 }, { 100, 150, 200 }, { 100, 200, 240 },
    { 80, 110, 140 },  { 70, 160, 225 }, { 80, 100, 130 },  { 100, 110, 190 },
};

u32 ObjTimeblock_CalculateIsVisible(ObjTimeblock* thisv) {
    if (!((thisv->dyna.actor.params >> 10) & 1)) {
        if (thisv->unk_177 == 0) {
            return thisv->unk_175;
        } else {
            u8 temp = ((thisv->dyna.actor.params >> 15) & 1) ? true : false;

            if (thisv->unk_177 == 1) {
                return thisv->unk_174 ^ temp;
            } else {
                u8 linkIsYoung = (LINK_AGE_IN_YEARS == YEARS_CHILD) ? true : false;

                return thisv->unk_174 ^ temp ^ linkIsYoung;
            }
        }
    } else {
        return (((thisv->dyna.actor.params >> 15) & 1) ? true : false) ^ thisv->unk_174;
    }
}

void ObjTimeblock_SpawnDemoEffect(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_EFFECT, thisv->dyna.actor.world.pos.x,
                thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z, 0, 0, 0,
                sSizeOptions[(thisv->dyna.actor.params >> 8) & 1].demoEffectParams);
}

void ObjTimeblock_ToggleSwitchFlag(GlobalContext* globalCtx, s32 flag) {
    if (Flags_GetSwitch(globalCtx, flag)) {
        Flags_UnsetSwitch(globalCtx, flag);
    } else {
        Flags_SetSwitch(globalCtx, flag);
    }
}

void ObjTimeblock_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjTimeblock* thisv = (ObjTimeblock*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    thisv->dyna.actor.world.rot.z = thisv->dyna.actor.shape.rot.z = 0;

    CollisionHeader_GetVirtual(&gSongOfTimeBlockCol, &colHeader);

    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    Actor_SetScale(&thisv->dyna.actor, sSizeOptions[(thisv->dyna.actor.params >> 8) & 1].scale);

    if ((thisv->dyna.actor.params >> 6) & 1) {
        thisv->unk_177 = 0;
    } else {
        thisv->unk_177 = ((thisv->dyna.actor.params & 0x3F) < 0x38) ? 2 : 1;
    }

    thisv->songObserverFunc = ObjTimeblock_WaitForOcarina;

    Actor_SetFocus(&thisv->dyna.actor, sSizeOptions[(thisv->dyna.actor.params >> 8) & 1].height);

    thisv->unk_174 = (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) ? true : false;
    thisv->unk_175 = ((thisv->dyna.actor.params >> 15) & 1) ? true : false;
    thisv->isVisible = ObjTimeblock_CalculateIsVisible(thisv);

    if (!((thisv->dyna.actor.params >> 10) & 1)) {
        ObjTimeblock_SetupNormal(thisv);
    } else if (thisv->isVisible) {
        ObjTimeblock_SetupAltBehaviorVisible(thisv);
    } else {
        ObjTimeblock_SetupAltBehaviourNotVisible(thisv);
    }

    // "Block of time"
    osSyncPrintf("時のブロック (<arg> %04xH <type> save:%d color:%d range:%d move:%d)\n", (u16)thisv->dyna.actor.params,
                 thisv->unk_177, thisv->dyna.actor.home.rot.z & 7, (thisv->dyna.actor.params >> 11) & 7,
                 (thisv->dyna.actor.params >> 10) & 1);
}

void ObjTimeblock_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjTimeblock* thisv = (ObjTimeblock*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

u8 ObjTimeblock_PlayerIsInRange(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    if (thisv->isVisible && func_80043590(&thisv->dyna)) {
        return false;
    }

    if (thisv->dyna.actor.xzDistToPlayer <= sRanges[(thisv->dyna.actor.params >> 11) & 7]) {
        Vec3f distance;
        f32 blockSize;

        func_8002DBD0(&thisv->dyna.actor, &distance, &GET_PLAYER(globalCtx)->actor.world.pos);
        blockSize = thisv->dyna.actor.scale.x * 50.0f + 6.0f;
        // Return true if player's xz position is not inside the block
        if (blockSize < fabsf(distance.x) || blockSize < fabsf(distance.z)) {
            return true;
        }
    }

    return false;
}

s32 ObjTimeblock_WaitForOcarina(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (ObjTimeblock_PlayerIsInRange(thisv, globalCtx)) {
        if (player->stateFlags2 & 0x1000000) {
            func_8010BD58(globalCtx, OCARINA_ACTION_FREE_PLAY);
            thisv->songObserverFunc = ObjTimeblock_WaitForSong;
        } else {
            player->stateFlags2 |= 0x800000;
        }
    }
    return false;
}

s32 ObjTimeblock_WaitForSong(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        thisv->songObserverFunc = ObjTimeblock_WaitForOcarina;
    }
    if (globalCtx->msgCtx.lastPlayedSong == OCARINA_SONG_TIME) {
        if (thisv->unk_172 == 254) {
            thisv->songEndTimer = 110;
        } else {
            thisv->songEndTimer--;
            if (thisv->songEndTimer == 0) {
                return true;
            }
        }
    }
    return false;
}

void ObjTimeblock_SetupDoNothing(ObjTimeblock* thisv) {
    thisv->actionFunc = ObjTimeblock_DoNothing;
}

void ObjTimeblock_DoNothing(ObjTimeblock* thisv, GlobalContext* globalCtx) {
}

void ObjTimeblock_SetupNormal(ObjTimeblock* thisv) {
    thisv->actionFunc = ObjTimeblock_Normal;
}

void ObjTimeblock_Normal(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    u32 newIsVisible;

    if (thisv->songObserverFunc(thisv, globalCtx) && thisv->demoEffectTimer <= 0) {
        ObjTimeblock_SpawnDemoEffect(thisv, globalCtx);
        thisv->demoEffectTimer = 160;

        // Possibly points the camera to thisv actor
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        // "◯◯◯◯ Time Block Attention Camera (frame counter  %d)\n"
        osSyncPrintf("◯◯◯◯ Time Block 注目カメラ (frame counter  %d)\n", globalCtx->state.frames);

        thisv->demoEffectFirstPartTimer = 12;

        if (thisv->unk_177 == 0) {
            thisv->dyna.actor.params ^= 0x8000;
        } else {
            ObjTimeblock_ToggleSwitchFlag(globalCtx, thisv->dyna.actor.params & 0x3F);
        }
    }

    thisv->unk_172 = globalCtx->msgCtx.lastPlayedSong;
    if (thisv->demoEffectFirstPartTimer > 0) {
        thisv->demoEffectFirstPartTimer--;
        if (thisv->demoEffectFirstPartTimer == 0) {
            if (thisv->unk_177 == 0) {
                thisv->unk_175 = ((thisv->dyna.actor.params >> 15) & 1) ? true : false;
            } else {
                thisv->unk_174 = (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) ? true : false;
            }
        }
    }

    newIsVisible = ObjTimeblock_CalculateIsVisible(thisv);
    if (thisv->unk_177 == 1 && newIsVisible != thisv->isVisible) {
        ObjTimeblock_SetupDoNothing(thisv);
    }
    thisv->isVisible = newIsVisible;

    if (thisv->demoEffectTimer == 50) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }
}

void func_80BA06AC(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    s32 switchFlag = thisv->dyna.actor.params & 0x3F;

    thisv->unk_172 = globalCtx->msgCtx.lastPlayedSong;

    if (thisv->demoEffectFirstPartTimer > 0 && --thisv->demoEffectFirstPartTimer == 0) {
        thisv->unk_174 = (Flags_GetSwitch(globalCtx, switchFlag)) ? true : false;
    }

    thisv->isVisible = ObjTimeblock_CalculateIsVisible(thisv);
    thisv->unk_176 = (Flags_GetSwitch(globalCtx, switchFlag)) ? true : false;
}

void ObjTimeblock_SetupAltBehaviorVisible(ObjTimeblock* thisv) {
    thisv->actionFunc = ObjTimeblock_AltBehaviorVisible;
}

void ObjTimeblock_AltBehaviorVisible(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    if (thisv->songObserverFunc(thisv, globalCtx) && thisv->demoEffectTimer <= 0) {
        thisv->demoEffectFirstPartTimer = 12;
        ObjTimeblock_SpawnDemoEffect(thisv, globalCtx);
        thisv->demoEffectTimer = 160;
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        // "Time Block Attention Camera (frame counter)"
        osSyncPrintf("◯◯◯◯ Time Block 注目カメラ (frame counter  %d)\n", globalCtx->state.frames);
        ObjTimeblock_ToggleSwitchFlag(globalCtx, thisv->dyna.actor.params & 0x3F);
    }

    func_80BA06AC(thisv, globalCtx);

    if (thisv->demoEffectTimer == 50) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }

    if (!thisv->isVisible && thisv->demoEffectTimer <= 0) {
        ObjTimeblock_SetupAltBehaviourNotVisible(thisv);
    }
}

void ObjTimeblock_SetupAltBehaviourNotVisible(ObjTimeblock* thisv) {
    thisv->actionFunc = ObjTimeblock_AltBehaviourNotVisible;
}

void ObjTimeblock_AltBehaviourNotVisible(ObjTimeblock* thisv, GlobalContext* globalCtx) {
    s32 switchFlag = thisv->dyna.actor.params & 0x3F;
    s8 switchFlagIsSet = (Flags_GetSwitch(globalCtx, switchFlag)) ? true : false;

    if (thisv->unk_176 ^ switchFlagIsSet && switchFlagIsSet ^ (((thisv->dyna.actor.params >> 15) & 1) ? true : false)) {
        if (thisv->demoEffectTimer <= 0) {
            ObjTimeblock_SpawnDemoEffect(thisv, globalCtx);
            thisv->demoEffectTimer = 160;
        }
        thisv->demoEffectFirstPartTimer = 12;
    }

    func_80BA06AC(thisv, globalCtx);

    if (thisv->isVisible && thisv->demoEffectTimer <= 0) {
        ObjTimeblock_SetupAltBehaviorVisible(thisv);
    }
}

void ObjTimeblock_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjTimeblock* thisv = (ObjTimeblock*)thisx;

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->demoEffectTimer > 0) {
        thisv->demoEffectTimer--;
    }

    if (thisv->isVisible) {
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    } else {
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}

void ObjTimeblock_Draw(Actor* thisx, GlobalContext* globalCtx) {
    if (((ObjTimeblock*)thisx)->isVisible) {
        Color_RGB8* primColor = &sPrimColors[thisx->home.rot.z & 7];

        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_timeblock.c", 762);

        func_80093D18(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_timeblock.c", 766),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, primColor->r, primColor->g, primColor->b, 255);
        gSPDisplayList(POLY_OPA_DISP++, gSongOfTimeBlockDL);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_timeblock.c", 772);
    }
}
