/*
 * File: z_bg_po_event.c
 * Overlay: ovl_Bg_Po_Event
 * Description: Poe sisters' paintings and puzzle blocks
 */

#include "z_bg_po_event.h"
#include "objects/object_po_sisters/object_po_sisters.h"

#define FLAGS 0

void BgPoEvent_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgPoEvent_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgPoEvent_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgPoEvent_Draw(Actor* thisvx, GlobalContext* globalCtx);
void BgPoEvent_Reset(void);

void BgPoEvent_BlockWait(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_BlockShake(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_BlockFall(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_BlockIdle(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_BlockPush(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_BlockReset(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_BlockSolved(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_AmyWait(BgPoEvent* thisv, GlobalContext* globalCtx); // Amy is the green Poe
void BgPoEvent_AmyPuzzle(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_PaintingEmpty(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_PaintingAppear(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_PaintingPresent(BgPoEvent* thisv, GlobalContext* globalCtx);
void BgPoEvent_PaintingBurn(BgPoEvent* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Po_Event_InitVars = {
    ACTOR_BG_PO_EVENT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_PO_SISTERS,
    sizeof(BgPoEvent),
    (ActorFunc)BgPoEvent_Init,
    (ActorFunc)BgPoEvent_Destroy,
    (ActorFunc)BgPoEvent_Update,
    (ActorFunc)BgPoEvent_Draw,
    (ActorResetFunc)BgPoEvent_Reset,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 25.0f, 33.0f, 0.0f }, { -25.0f, 33.0f, 0.0f }, { -25.0f, -33.0f, 0.0f } } },
    },
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 25.0f, 33.0f, 0.0f }, { -25.0f, -33.0f, 0.0f }, { 25.0f, -33.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static u8 sBlocksAtRest = 0;

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

static u8 sPuzzleState;

void BgPoEvent_InitPaintings(BgPoEvent* thisv, GlobalContext* globalCtx) {
    static s16 paintingPosX[] = { -1302, -866, 1421, 985 };
    static s16 paintingPosY[] = { 1107, 1091 };
    static s16 paintingPosZ[] = { -3384, -3252 };
    ColliderTrisElementInit* item;
    Vec3f* vtxVec;
    s32 i1;
    s32 i2;
    Vec3f sp9C[3];
    f32 coss;
    f32 sins;
    f32 scaleY;
    s32 phi_t2;
    Actor* newPainting;

    sins = Math_SinS(thisv->dyna.actor.shape.rot.y);
    coss = Math_CosS(thisv->dyna.actor.shape.rot.y);
    if (thisv->type == 4) {
        sins *= 2.4f;
        scaleY = 1.818f;
        coss *= 2.4f;
    } else {
        scaleY = 1.0f;
    }
    for (i1 = 0; i1 < sTrisInit.count; i1++) {
        item = &sTrisInit.elements[i1];
        if (1) {} // This section looks like a macro of some sort.
        for (i2 = 0; i2 < 3; i2++) {
            vtxVec = &item->dim.vtx[i2];
            sp9C[i2].x = (vtxVec->x * coss) + (thisv->dyna.actor.home.pos.x + (sins * vtxVec->z));
            sp9C[i2].y = (vtxVec->y * scaleY) + thisv->dyna.actor.home.pos.y;
            sp9C[i2].z = thisv->dyna.actor.home.pos.z + (coss * vtxVec->z) - (vtxVec->x * sins);
        }
        Collider_SetTrisVertices(&thisv->collider, i1, &sp9C[0], &sp9C[1], &sp9C[2]);
    }
    if ((thisv->type != 4) && (thisv->index != 2)) {
        phi_t2 = (thisv->type == 2) ? thisv->index : thisv->index + 2;
        newPainting = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_BG_PO_EVENT,
                                         paintingPosX[phi_t2], paintingPosY[thisv->index], paintingPosZ[thisv->index], 0,
                                         thisv->dyna.actor.shape.rot.y + 0x8000, 0,
                                         ((thisv->index + 1) << 0xC) + (thisv->type << 8) + thisv->dyna.actor.params);
        if (newPainting == NULL) {
            Actor_Kill(&thisv->dyna.actor);
            return;
        }
        if (thisv->index == 0) {
            if (thisv->dyna.actor.child->child == NULL) {
                Actor_Kill(&thisv->dyna.actor);
                return;
            }
            thisv->dyna.actor.parent = thisv->dyna.actor.child->child;
            thisv->dyna.actor.child->child->child = &thisv->dyna.actor;
        }
    }
    thisv->timer = 0;
    if (thisv->type == 4) {
        sPuzzleState = 0;
        thisv->actionFunc = BgPoEvent_AmyWait;
    } else {
        sPuzzleState = (s32)(Rand_ZeroOne() * 3.0f) % 3;
        thisv->actionFunc = BgPoEvent_PaintingEmpty;
    }
}

void BgPoEvent_InitBlocks(BgPoEvent* thisv, GlobalContext* globalCtx) {
    static s16 blockPosX[] = { 2149, 1969, 1909 };
    static s16 blockPosZ[] = { -1410, -1350, -1530 };
    Actor* newBlock;
    const CollisionHeader* colHeader = NULL;
    s32 bgId;

    thisv->dyna.actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    CollisionHeader_GetVirtual(&gPoSistersAmyBlockCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if ((thisv->type == 0) && (thisv->index != 3)) {
        newBlock = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_BG_PO_EVENT,
                                      blockPosX[thisv->index], thisv->dyna.actor.world.pos.y, blockPosZ[thisv->index], 0,
                                      thisv->dyna.actor.shape.rot.y, thisv->dyna.actor.shape.rot.z - 0x4000,
                                      ((thisv->index + 1) << 0xC) + (thisv->type << 8) + thisv->dyna.actor.params);
        if (newBlock == NULL) {
            Actor_Kill(&thisv->dyna.actor);
            return;
        }
        if (thisv->index == 0) {
            if (thisv->dyna.actor.child->child == NULL) {
                Actor_Kill(&thisv->dyna.actor);
                return;
            }
            if (thisv->dyna.actor.child->child->child == NULL) {
                Actor_Kill(&thisv->dyna.actor);
                Actor_Kill(thisv->dyna.actor.child);
                return;
            }
            thisv->dyna.actor.parent = thisv->dyna.actor.child->child->child;
            thisv->dyna.actor.child->child->child->child = &thisv->dyna.actor;
        }
    }
    thisv->dyna.actor.world.pos.y = 833.0f;
    thisv->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->dyna.actor.floorPoly, &bgId,
                                                               &thisv->dyna.actor, &thisv->dyna.actor.world.pos);
    thisv->actionFunc = BgPoEvent_BlockWait;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgPoEvent_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgPoEvent* thisv = (BgPoEvent*)thisvx;

    Actor_ProcessInitChain(thisvx, sInitChain);
    thisv->type = (thisvx->params >> 8) & 0xF;
    thisv->index = (thisvx->params >> 0xC) & 0xF;
    thisvx->params &= 0x3F;

    if (thisv->type >= 2) {
        Collider_InitTris(globalCtx, &thisv->collider);
        Collider_SetTris(globalCtx, &thisv->collider, thisvx, &sTrisInit, thisv->colliderItems);
        if (Flags_GetSwitch(globalCtx, thisvx->params)) {
            Actor_Kill(thisvx);
        } else {
            BgPoEvent_InitPaintings(thisv, globalCtx);
        }
    } else {
        DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
        if (Flags_GetSwitch(globalCtx, thisvx->params)) {
            Actor_Kill(thisvx);
        } else {
            BgPoEvent_InitBlocks(thisv, globalCtx);
        }
    }
}

void BgPoEvent_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgPoEvent* thisv = (BgPoEvent*)thisvx;

    if (thisv->type >= 2) {
        Collider_DestroyTris(globalCtx, &thisv->collider);
    } else {
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        if ((thisv->type == 1) && (gSaveContext.timer1Value > 0)) {
            gSaveContext.timer1State = 0xA;
        }
    }
}

void BgPoEvent_BlockWait(BgPoEvent* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.world.pos.y = 833.0f;
    if (sPuzzleState == 0x3F) {
        if (thisv->type == 1) {
            OnePointCutscene_Init(globalCtx, 3150, 65, NULL, MAIN_CAM);
        }
        thisv->timer = 45;
        thisv->actionFunc = BgPoEvent_BlockShake;
    } else if (thisv->dyna.actor.xzDistToPlayer > 50.0f) {
        if (thisv->type != 1) {
            sPuzzleState |= (1 << thisv->index);
        } else {
            sPuzzleState |= 0x10;
        }
    } else if (thisv->type != 1) {
        sPuzzleState &= ~(1 << thisv->index);
    } else {
        sPuzzleState &= ~0x10;
    }
}

void BgPoEvent_BlockShake(BgPoEvent* thisv, GlobalContext* globalCtx) {
    DECR(thisv->timer);
    if (thisv->timer < 15) {
        thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x + 2.0f * ((thisv->timer % 3) - 1);
        if (!(thisv->timer % 4)) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
        }
    }
    if (thisv->timer == 0) {
        thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x;
        sPuzzleState = 0;
        thisv->timer = 60;
        thisv->actionFunc = BgPoEvent_BlockFall;
    }
}

void BgPoEvent_CheckBlock(BgPoEvent* thisv) {
    s32 phi_v1;
    s32 phi_a1;
    s32 phi_t0;
    s32 phi_a3;

    if ((thisv->index == 3) || (thisv->index == 1)) {
        phi_v1 = thisv->dyna.actor.world.pos.z;
        phi_a1 = thisv->dyna.actor.child->world.pos.z;
        if (thisv->index == 3) {
            phi_a3 = thisv->dyna.actor.world.pos.x;
            phi_t0 = thisv->dyna.actor.child->world.pos.x;
        } else { // thisv->index == 1
            phi_a3 = thisv->dyna.actor.child->world.pos.x;
            phi_t0 = thisv->dyna.actor.world.pos.x;
        }
    } else {
        phi_v1 = thisv->dyna.actor.world.pos.x;
        phi_a1 = thisv->dyna.actor.child->world.pos.x;
        if (thisv->index == 0) {
            phi_a3 = thisv->dyna.actor.world.pos.z;
            phi_t0 = thisv->dyna.actor.child->world.pos.z;
        } else { // thisv->index == 2
            phi_a3 = thisv->dyna.actor.child->world.pos.z;
            phi_t0 = thisv->dyna.actor.world.pos.z;
        }
    }
    if ((phi_v1 == phi_a1) && ((phi_t0 - phi_a3) == 60)) {
        sPuzzleState |= (1 << thisv->index);
    } else {
        sPuzzleState &= ~(1 << thisv->index);
    }
}

void BgPoEvent_BlockFall(BgPoEvent* thisv, GlobalContext* globalCtx) {
    static s32 firstFall = 0;

    thisv->dyna.actor.velocity.y++;
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, 433.0f, thisv->dyna.actor.velocity.y)) {
        thisv->dyna.actor.flags &= ~ACTOR_FLAG_5;
        thisv->dyna.actor.velocity.y = 0.0f;
        sBlocksAtRest++;
        if (thisv->type != 1) {
            BgPoEvent_CheckBlock(thisv);
        } else {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_STONE_BOUND);
            func_80033E88(&thisv->dyna.actor, globalCtx, 5, 5);
            func_80088B34(thisv->timer);
            if (firstFall == 0) {
                firstFall = 1;
            } else {
                func_8002DF54(globalCtx, &GET_PLAYER(globalCtx)->actor, 7);
            }
        }
        thisv->direction = 0;
        thisv->actionFunc = BgPoEvent_BlockIdle;
    }
}

void BgPoEvent_BlockIdle(BgPoEvent* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* amy;

    if (sPuzzleState == 0xF) {
        thisv->actionFunc = BgPoEvent_BlockSolved;
        if ((thisv->type == 0) && (thisv->index == 0)) {
            amy =
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, thisv->dyna.actor.world.pos.x + 30.0f,
                            thisv->dyna.actor.world.pos.y - 30.0f, thisv->dyna.actor.world.pos.z + 30.0f, 0,
                            thisv->dyna.actor.shape.rot.y, 0, thisv->dyna.actor.params + 0x300);
            if (amy != NULL) {
                OnePointCutscene_Init(globalCtx, 3170, 30, amy, MAIN_CAM);
            }
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            gSaveContext.timer1State = 0xA;
        }
    } else {
        if ((gSaveContext.timer1Value == 0) && (sBlocksAtRest == 5)) {
            player->stateFlags2 &= ~0x10;
            sPuzzleState = 0x10;
            sBlocksAtRest = 0;
        }
        if ((sPuzzleState == 0x40) || ((sPuzzleState == 0x10) && !Player_InCsMode(globalCtx))) {
            thisv->dyna.actor.world.rot.z = thisv->dyna.actor.shape.rot.z;
            thisv->actionFunc = BgPoEvent_BlockReset;
            if (sPuzzleState == 0x10) {
                sPuzzleState = 0x40;
                Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_RISING);
                func_8002DF54(globalCtx, &player->actor, 8);
            }
        } else if (thisv->dyna.unk_150 != 0.0f) {
            if (thisv->direction == 0) {
                if (func_800435D8(globalCtx, &thisv->dyna, 0x1E, 0x32, -0x14) != 0) {
                    sBlocksAtRest--;
                    thisv->direction = (thisv->dyna.unk_150 >= 0.0f) ? 1.0f : -1.0f;
                    thisv->actionFunc = BgPoEvent_BlockPush;
                } else {
                    player->stateFlags2 &= ~0x10;
                    thisv->dyna.unk_150 = 0.0f;
                }
            } else {
                player->stateFlags2 &= ~0x10;
                thisv->dyna.unk_150 = 0.0f;
                DECR(thisv->direction);
            }
        } else {
            thisv->direction = 0;
        }
    }
}

static f32 blockPushDist = 0.0f;
void BgPoEvent_BlockPush(BgPoEvent* thisv, GlobalContext* globalCtx) {
    f32 displacement;
    s32 blockStop;
    Player* player = GET_PLAYER(globalCtx);

    thisv->dyna.actor.speedXZ += 0.1f;
    thisv->dyna.actor.speedXZ = CLAMP_MAX(thisv->dyna.actor.speedXZ, 2.0f);
    blockStop = Math_StepToF(&blockPushDist, 20.0f, thisv->dyna.actor.speedXZ);
    displacement = thisv->direction * blockPushDist;
    thisv->dyna.actor.world.pos.x = (Math_SinS(thisv->dyna.unk_158) * displacement) + thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z = (Math_CosS(thisv->dyna.unk_158) * displacement) + thisv->dyna.actor.home.pos.z;
    if (blockStop) {
        player->stateFlags2 &= ~0x10;
        if ((thisv->dyna.unk_150 > 0.0f) && (func_800435D8(globalCtx, &thisv->dyna, 0x1E, 0x32, -0x14) == 0)) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }
        thisv->dyna.unk_150 = 0.0f;
        thisv->dyna.actor.home.pos.x = thisv->dyna.actor.world.pos.x;
        thisv->dyna.actor.home.pos.z = thisv->dyna.actor.world.pos.z;
        blockPushDist = 0.0f;
        thisv->dyna.actor.speedXZ = 0.0f;
        thisv->direction = 5;
        sBlocksAtRest++;
        thisv->actionFunc = BgPoEvent_BlockIdle;
        if (thisv->type == 1) {
            return;
        }
        BgPoEvent_CheckBlock(thisv);
        BgPoEvent_CheckBlock((BgPoEvent*)thisv->dyna.actor.parent);
    }
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void BgPoEvent_BlockReset(BgPoEvent* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~0x10;
        thisv->dyna.unk_150 = 0.0f;
    }
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, 493.0f, 1.0f) &&
        Math_ScaledStepToS(&thisv->dyna.actor.shape.rot.z, thisv->dyna.actor.world.rot.z - 0x4000, 0x400)) {

        thisv->index = (thisv->index + 1) % 4;
        thisv->actionFunc = BgPoEvent_BlockFall;
        sPuzzleState = 0;
        if (thisv->type == 1) {
            thisv->timer += 10;
            thisv->timer = CLAMP_MAX(thisv->timer, 120);
        }
    }
}

void BgPoEvent_BlockSolved(BgPoEvent* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~0x10;
    }
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, 369.0f, 2.0f)) {
        sPuzzleState = 0x20;
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgPoEvent_AmyWait(BgPoEvent* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        sPuzzleState |= 0x20;
        thisv->timer = 5;
        Actor_SetColorFilter(&thisv->dyna.actor, 0x4000, 0xFF, 0, 5);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_PO_LAUGH2);
        thisv->actionFunc = BgPoEvent_AmyPuzzle;
    }
}

void BgPoEvent_AmyPuzzle(BgPoEvent* thisv, GlobalContext* globalCtx) {
    Vec3f pos;

    if (sPuzzleState == 0xF) {
        pos.x = thisv->dyna.actor.world.pos.x - 5.0f;
        pos.y = Rand_CenteredFloat(120.0f) + thisv->dyna.actor.world.pos.y;
        pos.z = Rand_CenteredFloat(120.0f) + thisv->dyna.actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &pos, &sZeroVec, &sZeroVec, 170, 0, 200, 255, 100, 170, 0, 255, 0, 1, 9, true);
    } else if (sPuzzleState == 0x20) {
        Actor_Kill(&thisv->dyna.actor);
    } else {
        DECR(thisv->timer);
    }
}

s32 BgPoEvent_NextPainting(BgPoEvent* thisv) {
    if ((thisv->dyna.actor.parent != NULL) && (thisv->dyna.actor.child != NULL)) {
        if (Rand_ZeroOne() < 0.5f) {
            sPuzzleState = ((BgPoEvent*)thisv->dyna.actor.parent)->index;
        } else {
            sPuzzleState = ((BgPoEvent*)thisv->dyna.actor.child)->index;
        }
    } else if (thisv->dyna.actor.parent != NULL) {
        sPuzzleState = ((BgPoEvent*)thisv->dyna.actor.parent)->index;
    } else if (thisv->dyna.actor.child != NULL) {
        sPuzzleState = ((BgPoEvent*)thisv->dyna.actor.child)->index;
    } else {
        return false;
    }
    return true;
}

void BgPoEvent_PaintingEmpty(BgPoEvent* thisv, GlobalContext* globalCtx) {
    if (sPuzzleState == thisv->index) {
        thisv->timer = 255;
        thisv->actionFunc = BgPoEvent_PaintingAppear;
    }
}

void BgPoEvent_PaintingAppear(BgPoEvent* thisv, GlobalContext* globalCtx) {
    thisv->timer -= 20;
    if (thisv->timer <= 0) {
        thisv->timer = 1000;
        thisv->actionFunc = BgPoEvent_PaintingPresent;
    }
}

void BgPoEvent_PaintingVanish(BgPoEvent* thisv, GlobalContext* globalCtx) {
    thisv->timer += 20;
    if (thisv->timer >= 255) {
        BgPoEvent_NextPainting(thisv);
        thisv->actionFunc = BgPoEvent_PaintingEmpty;
    }
}

void BgPoEvent_PaintingPresent(BgPoEvent* thisv, GlobalContext* globalCtx) {
    Actor* thisvx = &thisv->dyna.actor;
    Player* player = GET_PLAYER(globalCtx);

    DECR(thisv->timer);

    if (((thisv->timer == 0) || ((thisvx->xzDistToPlayer < 150.0f) && (thisvx->yDistToPlayer < 50.0f)) ||
         (func_8002DD78(player) && (thisvx->xzDistToPlayer < 320.0f) &&
          ((thisv->index != 2) ? (thisvx->yDistToPlayer < 100.0f) : (thisvx->yDistToPlayer < 0.0f)) &&
          Player_IsFacingActor(thisvx, 0x2000, globalCtx))) &&
        ((thisvx->parent != NULL) || (thisvx->child != NULL))) {
        /*The third condition in the || is checking if
            1) Link is holding a ranged weapon
            2) Link is too close in the xz plane
            3) Link is too close in the y direction. The painting
               under the balcony allows him to be closer.
            4) Link is within 45 degrees of facing the painting. */
        thisv->timer = 0;
        Audio_PlayActorSound2(thisvx, NA_SE_EN_PO_LAUGH);
        thisv->actionFunc = BgPoEvent_PaintingVanish;
    } else if (thisv->collider.base.acFlags & AC_HIT) {
        if (!BgPoEvent_NextPainting(thisv)) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, thisvx->world.pos.x,
                        thisvx->world.pos.y - 40.0f, thisvx->world.pos.z, 0, thisvx->shape.rot.y, 0,
                        thisvx->params + ((thisv->type - 1) << 8));
            OnePointCutscene_Init(globalCtx, 3160, 80, thisvx, MAIN_CAM);
            func_80078884(NA_SE_SY_CORRECT_CHIME);

        } else {
            Audio_PlayActorSound2(thisvx, NA_SE_EN_PO_LAUGH2);
            OnePointCutscene_Init(globalCtx, 3160, 35, thisvx, MAIN_CAM);
        }
        if (thisvx->parent != NULL) {
            thisvx->parent->child = NULL;
            thisvx->parent = NULL;
        }
        if (thisvx->child != NULL) {
            thisvx->child->parent = NULL;
            thisvx->child = NULL;
        }
        thisv->timer = 20;
        thisv->actionFunc = BgPoEvent_PaintingBurn;
    }
}

void BgPoEvent_PaintingBurn(BgPoEvent* thisv, GlobalContext* globalCtx) {
    Vec3f sp54;

    thisv->timer--;
    sp54.x = (Math_SinS(thisv->dyna.actor.shape.rot.y) * 5.0f) + thisv->dyna.actor.world.pos.x;
    sp54.y = Rand_CenteredFloat(66.0f) + thisv->dyna.actor.world.pos.y;
    sp54.z = Rand_CenteredFloat(50.0f) + thisv->dyna.actor.world.pos.z;
    if (thisv->timer >= 0) {
        if (thisv->type == 2) {
            EffectSsDeadDb_Spawn(globalCtx, &sp54, &sZeroVec, &sZeroVec, 100, 0, 255, 255, 150, 170, 255, 0, 0, 1, 9,
                                 true);
        } else {
            EffectSsDeadDb_Spawn(globalCtx, &sp54, &sZeroVec, &sZeroVec, 100, 0, 200, 255, 255, 170, 50, 100, 255, 1, 9,
                                 true);
        }
    }
    if (thisv->timer == 0) {
        thisv->dyna.actor.draw = NULL;
    }
    if (thisv->timer < -60) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgPoEvent_Update(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgPoEvent* thisv = (BgPoEvent*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
    if ((thisv->actionFunc == BgPoEvent_AmyWait) || (thisv->actionFunc == BgPoEvent_PaintingPresent)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void BgPoEvent_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    static const Gfx* displayLists[] = {
        gPoSistersAmyBlockDL,     gPoSistersAmyBethBlockDL, gPoSistersJoellePaintingDL,
        gPoSistersBethPaintingDL, gPoSistersAmyPaintingDL,
    };
    s32 pad;
    BgPoEvent* thisv = (BgPoEvent*)thisvx;
    u8 alpha;
    Vec3f sp58;
    Vec3f sp4C;
    f32 sp48;
    s32 pad2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_po_event.c", 1481);
    func_80093D18(globalCtx->state.gfxCtx);
    if ((thisv->type == 3) || (thisv->type == 2)) {
        if (thisv->actionFunc == BgPoEvent_PaintingEmpty) {
            alpha = 255;
        } else if (thisv->actionFunc == BgPoEvent_PaintingPresent) {
            alpha = 0;
        } else {
            alpha = thisv->timer;
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, alpha);
    }
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_po_event.c", 1501),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, displayLists[thisv->type]);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_po_event.c", 1508);

    if ((thisv->type == 0) || (thisv->type == 1)) {
        sp48 = (833.0f - thisv->dyna.actor.world.pos.y) * 0.0025f;
        if (!(sp48 > 1.0f)) {
            sp58.x = thisv->dyna.actor.world.pos.x;
            sp58.y = thisv->dyna.actor.world.pos.y - 30.0f;
            sp58.z = thisv->dyna.actor.world.pos.z;
            sp4C.y = 1.0f;
            sp4C.x = sp4C.z = (sp48 * 0.3f) + 0.4f;
            func_80033C30(&sp58, &sp4C, (u8)(155.0f + sp48 * 100.0f), globalCtx);
        }
    }
}

void BgPoEvent_Reset(void) {
    sBlocksAtRest = 0;
    sPuzzleState = 0;
    blockPushDist = 0.0f;
}