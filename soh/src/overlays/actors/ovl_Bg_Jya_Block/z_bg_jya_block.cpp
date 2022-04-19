/*
 * File: z_bg_jya_block.c
 * Overlay: ovl_Bg_Jya_Block
 * Description: Silver Block (Child Link)
 */

#include "z_bg_jya_block.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS 0

void BgJyaBlock_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaBlock_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaBlock_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaBlock_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Bg_Jya_Block_InitVars = {
    ACTOR_BG_JYA_BLOCK,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(BgJyaBlock),
    (ActorFunc)BgJyaBlock_Init,
    (ActorFunc)BgJyaBlock_Destroy,
    (ActorFunc)BgJyaBlock_Update,
    (ActorFunc)BgJyaBlock_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 333, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

void BgJyaBlock_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgJyaBlock* thisv = (BgJyaBlock*)thisx;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, 0);
    CollisionHeader_GetVirtual(&gPushBlockCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    if ((LINK_AGE_IN_YEARS != YEARS_CHILD) || !Flags_GetSwitch(globalCtx, thisx->params & 0x3F)) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgJyaBlock_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaBlock* thisv = (BgJyaBlock*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgJyaBlock_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaBlock* thisv = (BgJyaBlock*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    player->stateFlags2 &= ~0x10;
    thisv->dyna.unk_150 = 0.0f;
}

void BgJyaBlock_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_block.c", 145);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gPushBlockGrayTex));
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_block.c", 153),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetEnvColor(POLY_OPA_DISP++, 232, 210, 176, 255);
    gSPDisplayList(POLY_OPA_DISP++, gPushBlockDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_block.c", 158);
}
