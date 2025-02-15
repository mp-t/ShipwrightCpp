/*
 * File: z_en_scene_change.c
 * Overlay: ovl_En_Scene_Change
 * Description: Unknown (Broken Actor)
 */

#include "z_en_scene_change.h"

#define FLAGS 0

void EnSceneChange_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSceneChange_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSceneChange_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSceneChange_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnSceneChange_DoNothing(EnSceneChange* thisv, GlobalContext* globalCtx);

ActorInit En_Scene_Change_InitVars = {
    ACTOR_EN_SCENE_CHANGE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_JJ,
    sizeof(EnSceneChange),
    (ActorFunc)EnSceneChange_Init,
    (ActorFunc)EnSceneChange_Destroy,
    (ActorFunc)EnSceneChange_Update,
    (ActorFunc)EnSceneChange_Draw,
    NULL,
};

void EnSceneChange_SetupAction(EnSceneChange* thisv, EnSceneChangeActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnSceneChange_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSceneChange* thisv = (EnSceneChange*)thisx;

    EnSceneChange_SetupAction(thisv, EnSceneChange_DoNothing);
}

void EnSceneChange_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnSceneChange_DoNothing(EnSceneChange* thisv, GlobalContext* globalCtx) {
}

void EnSceneChange_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSceneChange* thisv = (EnSceneChange*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void EnSceneChange_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad[2];
    Gfx* displayList;
    s32 pad2[2];
    Gfx* displayListHead;

    displayList = static_cast<Gfx*>(Graph_Alloc(globalCtx->state.gfxCtx, 0x3C0));

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_scene_change.c", 290);

    displayListHead = displayList;
    gSPSegment(POLY_OPA_DISP++, 0x0C, displayListHead);

    func_80093D18(globalCtx->state.gfxCtx);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_scene_change.c", 386);
}
