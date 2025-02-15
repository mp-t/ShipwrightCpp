/*
 * File: z_demo_geff.c
 * Overlay: Demo_Geff
 * Description: Ganon's Lair Rubble Fragment
 */

#include "z_demo_geff.h"
#include "objects/object_geff/object_geff.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void DemoGeff_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoGeff_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoGeff_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoGeff_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80978030(DemoGeff* thisv, GlobalContext* globalCtx);

void func_809783D4(DemoGeff* thisv, GlobalContext* globalCtx);
void func_80978308(DemoGeff* thisv, GlobalContext* globalCtx);

void func_809784D4(DemoGeff* thisv, GlobalContext* globalCtx);
void func_80978344(DemoGeff* thisv, GlobalContext* globalCtx);

static s16 sObjectIDs[] = {
    OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF,
};

static DemoGeffInitFunc sInitFuncs[] = {
    func_80978030, func_80978030, func_80978030, func_80978030, func_80978030,
    func_80978030, func_80978030, func_80978030, func_80978030,
};

static DemoGeffActionFunc sActionFuncs[] = {
    func_809783D4,
    func_80978308,
};

static DemoGeffDrawFunc sDrawFuncs[] = {
    func_809784D4,
    func_80978344,
};

ActorInit Demo_Geff_InitVars = {
    ACTOR_DEMO_GEFF,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GEFF,
    sizeof(DemoGeff),
    (ActorFunc)DemoGeff_Init,
    (ActorFunc)DemoGeff_Destroy,
    (ActorFunc)DemoGeff_Update,
    (ActorFunc)DemoGeff_Draw,
    NULL,
};

void DemoGeff_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void DemoGeff_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoGeff* thisv = (DemoGeff*)thisx;

    if (thisv->actor.params < 0 || thisv->actor.params >= 9) {
        osSyncPrintf(VT_FGCOL(RED) "Demo_Geff_Actor_ct:arg_dataがおかしい!!!!!!!!!!!!\n" VT_RST);
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->action = 0;
    thisv->drawConfig = 0;
}

void func_80977EA8(GlobalContext* globalCtx, const Gfx* dlist) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_geff.c", 181);

    func_80093D18(gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx, "../z_demo_geff.c", 183),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, dlist);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_geff.c", 188);
}

void func_80977F80(DemoGeff* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s32 objBankIndex = thisv->objBankIndex;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_geff.c", 204);

    gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[objBankIndex].segment);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[objBankIndex].segment);

    // Necessary to match
    if (!globalCtx) {}

    CLOSE_DISPS(gfxCtx, "../z_demo_geff.c", 212);
}

void func_80978030(DemoGeff* thisv, GlobalContext* globalCtx) {
    Vec3f* thisScale = &thisv->actor.scale;

    thisv->action = 1;
    thisv->drawConfig = 1;

    switch (thisv->actor.params) {
        case 0:
        case 3:
        case 6:
            thisScale->x = (kREG(7) * 0.01f) + 0.3f;
            thisScale->y = (kREG(8) * 0.01f) + 0.3f;
            thisScale->z = (kREG(9) * 0.01f) + 0.3f;
            break;
        case 1:
        case 4:
        case 7:
            thisScale->x = (kREG(10) * 0.01f) + 0.15f;
            thisScale->y = (kREG(11) * 0.01f) + 0.29f;
            thisScale->z = (kREG(12) * 0.01f) + 0.12f;
            break;
        default:
            thisScale->x = (kREG(13) * 0.01f) + 0.1f;
            thisScale->y = (kREG(14) * 0.01f) + 0.15f;
            thisScale->z = (kREG(15) * 0.01f) + 0.2f;
            break;
    }
}

void func_809781FC(DemoGeff* thisv, GlobalContext* globalCtx) {
    s32 targetParams = 2;
    Actor* propIt;

    if (thisv->demoGt == NULL) {
        propIt = globalCtx->actorCtx.actorLists[ACTORCAT_PROP].head;
        if ((thisv->actor.params != 0) && (thisv->actor.params != 1) && (thisv->actor.params != 2)) {
            targetParams = 2;
        } else {
            targetParams = 1;
        }
        while (propIt != NULL) {
            if (propIt->id == ACTOR_DEMO_GT && propIt->params == targetParams) {
                thisv->deltaPosX = thisv->actor.world.pos.x - propIt->world.pos.x;
                thisv->deltaPosY = thisv->actor.world.pos.y - propIt->world.pos.y;
                thisv->deltaPosZ = thisv->actor.world.pos.z - propIt->world.pos.z;
                thisv->demoGt = (DemoGt*)propIt;
            }
            propIt = propIt->next;
        }
    }
}

void func_809782A0(DemoGeff* thisv, GlobalContext* globalCtx) {
    DemoGt* demoGt = thisv->demoGt;
    s16 params = thisv->actor.params;

    if (demoGt != NULL && (params != 6) && (params != 7) && (params != 8)) {
        thisv->actor.world.pos.x = demoGt->dyna.actor.world.pos.x + thisv->deltaPosX;
        thisv->actor.world.pos.y = demoGt->dyna.actor.world.pos.y + thisv->deltaPosY;
        thisv->actor.world.pos.z = demoGt->dyna.actor.world.pos.z + thisv->deltaPosZ;
    }
}

void func_80978308(DemoGeff* thisv, GlobalContext* globalCtx) {
    func_809781FC(thisv, globalCtx);
    func_809782A0(thisv, globalCtx);
    func_80978030(thisv, globalCtx);
}

void func_80978344(DemoGeff* thisv, GlobalContext* globalCtx) {
    func_80977EA8(globalCtx, gGanonRubbleDL);
}

void func_80978370(DemoGeff* thisv, GlobalContext* globalCtx) {
    s16 params = thisv->actor.params;
    DemoGeffInitFunc initFunc = sInitFuncs[params];
    if (initFunc == NULL) {
        osSyncPrintf(VT_FGCOL(RED) " Demo_Geff_main_init:初期化処理がおかしいarg_data = %d!\n" VT_RST, params);
        Actor_Kill(&thisv->actor);
        return;
    }
    initFunc(thisv, globalCtx);
}

void func_809783D4(DemoGeff* thisv, GlobalContext* globalCtx) {
    ObjectContext* objCtx = &globalCtx->objectCtx;
    Actor* thisx = &thisv->actor;
    s32 params = thisx->params;
    s16 objectId = sObjectIDs[params];
    s32 objBankIndex = Object_GetIndex(objCtx, objectId);
    s32 pad;

    if (objBankIndex < 0) {
        osSyncPrintf(VT_FGCOL(RED) "Demo_Geff_main_bank:バンクを読めない arg_data = %d!\n" VT_RST, params);
        Actor_Kill(thisx);
        return;
    }
    if (Object_IsLoaded(objCtx, objBankIndex)) {
        thisv->objBankIndex = objBankIndex;
        func_80978370(thisv, globalCtx);
    }
}

void DemoGeff_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoGeff* thisv = (DemoGeff*)thisx;

    if (thisv->action < 0 || thisv->action >= 2 || sActionFuncs[thisv->action] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[thisv->action](thisv, globalCtx);
}

void func_809784D4(DemoGeff* thisv, GlobalContext* globalCtx) {
}

void DemoGeff_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoGeff* thisv = (DemoGeff*)thisx;
    s32 drawConfig = thisv->drawConfig;

    if (drawConfig < 0 || drawConfig >= 2 || sDrawFuncs[drawConfig] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    if (drawConfig != 0) {
        func_80977F80(thisv, globalCtx);
    }
    sDrawFuncs[drawConfig](thisv, globalCtx);
}
