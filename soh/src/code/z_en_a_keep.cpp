#include "global.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include <objects/object_d_hsblock/object_d_hsblock.h>

#define FLAGS ACTOR_FLAG_4

void EnAObj_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAObj_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAObj_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAObj_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnAObj_WaitFinishedTalking(EnAObj* thisv, GlobalContext* globalCtx);
void EnAObj_WaitTalk(EnAObj* thisv, GlobalContext* globalCtx);
void EnAObj_BlockRot(EnAObj* thisv, GlobalContext* globalCtx);
void EnAObj_BoulderFragment(EnAObj* thisv, GlobalContext* globalCtx);
void EnAObj_Block(EnAObj* thisv, GlobalContext* globalCtx);

void EnAObj_SetupWaitTalk(EnAObj* thisv, s16 type);
void EnAObj_SetupBlockRot(EnAObj* thisv, s16 type);
void EnAObj_SetupBoulderFragment(EnAObj* thisv, s16 type);
void EnAObj_SetupBlock(EnAObj* thisv, s16 type);

const ActorInit En_A_Obj_InitVars = {
    ACTOR_EN_A_OBJ,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnAObj),
    (ActorFunc)EnAObj_Init,
    (ActorFunc)EnAObj_Destroy,
    (ActorFunc)EnAObj_Update,
    (ActorFunc)EnAObj_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ALL,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 60, 0, { 0, 0, 0 } },
};

//extern CollisionHeader D_06000730; // gHookshotTargetCol ?

static const CollisionHeader* sColHeaders[] = {
    &gLargerCubeCol,       // A_OBJ_GRASS_CLUMP, A_OBJ_TREE_STUMP
    &gLargerCubeCol,       // A_OBJ_BLOCK_LARGE, A_OBJ_BLOCK_HUGE
    &gSmallerFlatBlockCol, // unused
    &gLargerFlatBlockCol,  // A_OBJ_BLOCK_SMALL_ROT, A_OBJ_BLOCK_LARGE_ROT
    &gSmallerCubeCol,      // unused
    //&D_06000730,           // A_OBJ_UNKNOWN_6 // OTRTODO
};

static const Gfx* sDLists[] = {
    gFlatBlockDL,
    gFlatBlockDL,
    gFlatBlockDL,
    gFlatRotBlockDL,
    gFlatRotBlockDL,
    gSmallCubeDL,
    gHookshotPostDL, /* gHookshotPostDL ? */ // 0x06000210, // OTRTODO!
    gGrassBladesDL,
    gTreeStumpDL,
    gSignRectangularDL,
    gSignDirectionalDL,
    gBoulderFragmentsDL,
};

void EnAObj_SetupAction(EnAObj* thisv, EnAObjActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnAObj_Init(Actor* thisx, GlobalContext* globalCtx) {
    const CollisionHeader* colHeader = NULL;
    s32 pad;
    EnAObj* thisv = (EnAObj*)thisx;
    f32 shadowScale = 6.0f;

    thisv->textId = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;

    switch (thisx->params) {
        case A_OBJ_BLOCK_SMALL:
            Actor_SetScale(thisx, 0.025f);
            break;
        case A_OBJ_BLOCK_LARGE:
            Actor_SetScale(thisx, 0.05f);
            break;
        case A_OBJ_BLOCK_HUGE:
        case A_OBJ_CUBE_SMALL:
        case A_OBJ_UNKNOWN_6:
            Actor_SetScale(thisx, 0.1f);
            break;
        case A_OBJ_BLOCK_SMALL_ROT:
            Actor_SetScale(thisx, 0.005f);
            break;
        case A_OBJ_BLOCK_LARGE_ROT:
        default:
            Actor_SetScale(thisx, 0.01f);
            break;
    }

    if (thisx->params >= A_OBJ_SIGNPOST_OBLONG) {
        shadowScale = 12.0f;
    }

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, shadowScale);

    thisx->focus.pos = thisx->world.pos;
    thisv->dyna.bgId = BGACTOR_NEG_ONE;
    thisv->dyna.unk_160 = 0;
    thisv->dyna.unk_15C = DPM_UNK;
    thisx->uncullZoneDownward = 1200.0f;
    thisx->uncullZoneScale = 200.0f;

    switch (thisx->params) {
        case A_OBJ_BLOCK_LARGE:
        case A_OBJ_BLOCK_HUGE:
            thisv->dyna.bgId = 1;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisx, ACTORCAT_BG);
            EnAObj_SetupBlock(thisv, thisx->params);
            break;
        case A_OBJ_BLOCK_SMALL_ROT:
        case A_OBJ_BLOCK_LARGE_ROT:
            thisv->dyna.bgId = 3;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisx, ACTORCAT_BG);
            EnAObj_SetupBlockRot(thisv, thisx->params);
            break;
        case A_OBJ_UNKNOWN_6:
            // clang-format off
            thisx->flags |= ACTOR_FLAG_0; thisv->dyna.bgId = 5; thisv->focusYoffset = 10.0f;
            // clang-format on
            thisx->gravity = -2.0f;
            EnAObj_SetupWaitTalk(thisv, thisx->params);
            break;
        case A_OBJ_GRASS_CLUMP:
        case A_OBJ_TREE_STUMP:
            thisv->dyna.bgId = 0;
            EnAObj_SetupWaitTalk(thisv, thisx->params);
            break;
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            thisx->textId = (thisv->textId & 0xFF) | 0x300;
            // clang-format off
            thisx->flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3; thisx->targetArrowOffset = 500.0f;
            // clang-format on
            thisv->focusYoffset = 45.0f;
            EnAObj_SetupWaitTalk(thisv, thisx->params);
            Collider_InitCylinder(globalCtx, &thisv->collider);
            Collider_SetCylinder(globalCtx, &thisv->collider, thisx, &sCylinderInit);
            thisx->colChkInfo.mass = MASS_IMMOVABLE;
            thisx->targetMode = 0;
            break;
        case A_OBJ_BOULDER_FRAGMENT:
            thisx->gravity = -1.5f;
            EnAObj_SetupBoulderFragment(thisv, thisx->params);
            break;
        default:
            thisx->gravity = -2.0f;
            EnAObj_SetupWaitTalk(thisv, thisx->params);
            break;
    }

    if (thisx->params <= A_OBJ_BLOCK_LARGE_ROT) { // A_OBJ_BLOCK_*
        thisx->colChkInfo.mass = MASS_IMMOVABLE;
    }

    if (thisv->dyna.bgId != BGACTOR_NEG_ONE) {
        CollisionHeader_GetVirtual(sColHeaders[thisv->dyna.bgId], &colHeader);
        thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
    }
}

void EnAObj_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnAObj* thisv = (EnAObj*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);

    switch (thisv->dyna.actor.params) {
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            Collider_DestroyCylinder(globalCtx, &thisv->collider);
            break;
    }
}

void EnAObj_WaitFinishedTalking(EnAObj* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->dyna.actor, globalCtx)) {
        EnAObj_SetupWaitTalk(thisv, thisv->dyna.actor.params);
    }
}

void EnAObj_SetupWaitTalk(EnAObj* thisv, s16 type) {
    EnAObj_SetupAction(thisv, EnAObj_WaitTalk);
}

void EnAObj_WaitTalk(EnAObj* thisv, GlobalContext* globalCtx) {
    s16 relYawTowardsPlayer;

    if (thisv->dyna.actor.textId != 0) {
        relYawTowardsPlayer = thisv->dyna.actor.yawTowardsPlayer - thisv->dyna.actor.shape.rot.y;
        if (ABS(relYawTowardsPlayer) < 0x2800 ||
            (thisv->dyna.actor.params == A_OBJ_SIGNPOST_ARROW && ABS(relYawTowardsPlayer) > 0x5800)) {
            if (Actor_ProcessTalkRequest(&thisv->dyna.actor, globalCtx)) {
                EnAObj_SetupAction(thisv, EnAObj_WaitFinishedTalking);
            } else {
                func_8002F2F4(&thisv->dyna.actor, globalCtx);
            }
        }
    }
}

void EnAObj_SetupBlockRot(EnAObj* thisv, s16 type) {
    thisv->rotateState = 0;
    thisv->rotateWaitTimer = 10;
    thisv->dyna.actor.world.rot.y = 0;
    thisv->dyna.actor.shape.rot = thisv->dyna.actor.world.rot;
    EnAObj_SetupAction(thisv, EnAObj_BlockRot);
}

void EnAObj_BlockRot(EnAObj* thisv, GlobalContext* globalCtx) {
    if (thisv->rotateState == 0) {
        if (thisv->dyna.unk_160 != 0) {
            thisv->rotateState++;
            thisv->rotateForTimer = 20;

            if ((s16)(thisv->dyna.actor.yawTowardsPlayer + 0x4000) < 0) {
                thisv->rotSpeedX = -0x3E8;
            } else {
                thisv->rotSpeedX = 0x3E8;
            }

            if (thisv->dyna.actor.yawTowardsPlayer < 0) {
                thisv->rotSpeedY = -thisv->rotSpeedX;
            } else {
                thisv->rotSpeedY = thisv->rotSpeedX;
            }
        }
    } else {
        if (thisv->rotateWaitTimer != 0) {
            thisv->rotateWaitTimer--;
        } else {
            thisv->dyna.actor.shape.rot.y += thisv->rotSpeedY;
            thisv->dyna.actor.shape.rot.x += thisv->rotSpeedX;
            thisv->rotateForTimer--;
            thisv->dyna.actor.gravity = -1.0f;

            if (thisv->rotateForTimer == 0) {
                thisv->dyna.actor.world.pos = thisv->dyna.actor.home.pos;
                thisv->rotateState = 0;
                thisv->rotateWaitTimer = 10;
                thisv->dyna.actor.velocity.y = 0.0f;
                thisv->dyna.actor.gravity = 0.0f;
                thisv->dyna.actor.shape.rot = thisv->dyna.actor.world.rot;
            }
        }
    }
}

void EnAObj_SetupBoulderFragment(EnAObj* thisv, s16 type) {
    EnAObj_SetupAction(thisv, EnAObj_BoulderFragment);
}

void EnAObj_BoulderFragment(EnAObj* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->dyna.actor.speedXZ, 1.0f, 1.0f, 0.5f, 0.0f);
    thisv->dyna.actor.shape.rot.x += thisv->dyna.actor.world.rot.x >> 1;
    thisv->dyna.actor.shape.rot.z += thisv->dyna.actor.world.rot.z >> 1;

    if (thisv->dyna.actor.speedXZ != 0.0f && thisv->dyna.actor.bgCheckFlags & 0x8) {
        thisv->dyna.actor.world.rot.y =
            thisv->dyna.actor.wallYaw - thisv->dyna.actor.world.rot.y + thisv->dyna.actor.wallYaw - 0x8000;
        if (1) {}
        thisv->dyna.actor.bgCheckFlags &= ~0x8;
    }

    if (thisv->dyna.actor.bgCheckFlags & 0x2) {
        if (thisv->dyna.actor.velocity.y < -8.0f) {
            thisv->dyna.actor.velocity.y *= -0.6f;
            thisv->dyna.actor.speedXZ *= 0.6f;
            thisv->dyna.actor.bgCheckFlags &= ~0x3;
        } else {
            Actor_Kill(&thisv->dyna.actor);
        }
    }
}

void EnAObj_SetupBlock(EnAObj* thisv, s16 type) {
    thisv->dyna.actor.uncullZoneDownward = 1200.0f;
    thisv->dyna.actor.uncullZoneScale = 720.0f;
    EnAObj_SetupAction(thisv, EnAObj_Block);
}

void EnAObj_Block(EnAObj* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.speedXZ += thisv->dyna.unk_150;
    thisv->dyna.actor.world.rot.y = thisv->dyna.unk_158;
    thisv->dyna.actor.speedXZ = CLAMP(thisv->dyna.actor.speedXZ, -2.5f, 2.5f);

    Math_SmoothStepToF(&thisv->dyna.actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);

    if (thisv->dyna.actor.speedXZ != 0.0f) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }

    thisv->dyna.unk_154 = 0.0f;
    thisv->dyna.unk_150 = 0.0f;
}

void EnAObj_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnAObj* thisv = (EnAObj*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->dyna.actor);

    if (thisv->dyna.actor.gravity != 0.0f) {
        if (thisv->dyna.actor.params != A_OBJ_BOULDER_FRAGMENT) {
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 5.0f, 40.0f, 0.0f, 0x1D);
        } else {
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 5.0f, 20.0f, 0.0f, 0x1D);
        }
    }

    thisv->dyna.actor.focus.pos = thisv->dyna.actor.world.pos;
    thisv->dyna.actor.focus.pos.y += thisv->focusYoffset;

    switch (thisv->dyna.actor.params) {
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->collider);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            break;
    }
}

void EnAObj_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 type = thisx->params;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_a_keep.c", 701);

    func_80093D18(globalCtx->state.gfxCtx);

    if (type >= A_OBJ_MAX) {
        type = A_OBJ_BOULDER_FRAGMENT;
    }

    if (thisx->params == A_OBJ_BOULDER_FRAGMENT) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 1, 60, 60, 60, 50);
    }

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_a_keep.c", 712),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, sDLists[type]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_a_keep.c", 715);
}
