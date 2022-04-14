/*
 * File: z_obj_hana.c
 * Overlay: Obj_Hana
 * Description: Grave Flower
 */

#include "z_obj_hana.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void ObjHana_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjHana_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjHana_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjHana_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Obj_Hana_InitVars = {
    ACTOR_OBJ_HANA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_FIELD_KEEP,
    sizeof(ObjHana),
    (ActorFunc)ObjHana_Init,
    (ActorFunc)ObjHana_Destroy,
    (ActorFunc)ObjHana_Update,
    (ActorFunc)ObjHana_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 8, 10, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

typedef struct {
    /* 0x00 */ const Gfx* dList;
    /* 0x04 */ f32 scale;
    /* 0x08 */ f32 yOffset;
    /* 0x0C */ s16 radius;
    /* 0x0E */ s16 height;
} HanaParams; // size = 0x10

static HanaParams sHanaParams[] = {
    { gHanaDL, 0.01f, 0.0f, -1, 0 },
    { gFieldKakeraDL, 0.1f, 58.0f, 10, 18 },
    { gFieldBushDL, 0.4f, 0.0f, 12, 44 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 60, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

void ObjHana_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjHana* thisv = (ObjHana*)thisx;
    s16 type = thisv->actor.params & 3;
    HanaParams* params = &sHanaParams[type];

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, params->scale);
    thisv->actor.shape.yOffset = params->yOffset;
    if (params->radius >= 0) {
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        thisv->collider.dim.radius = params->radius;
        thisv->collider.dim.height = params->height;
        CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    }

    if (type == 2 && (gSaveContext.eventChkInf[4] & 1)) {
        Actor_Kill(&thisv->actor);
    }
}

void ObjHana_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjHana* thisv = (ObjHana*)thisx;

    if (sHanaParams[thisv->actor.params & 3].radius >= 0) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void ObjHana_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjHana* thisv = (ObjHana*)thisx;

    if (sHanaParams[thisv->actor.params & 3].radius >= 0 && thisv->actor.xzDistToPlayer < 400.0f) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void ObjHana_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, sHanaParams[thisx->params & 3].dList);
}
