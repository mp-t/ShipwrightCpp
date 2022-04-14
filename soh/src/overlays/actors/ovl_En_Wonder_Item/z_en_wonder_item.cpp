/*
 * File: z_en_wonder_item.c
 * Overlay: ovl_En_Wonder_Item
 * Description: Invisible Collectable; Used in MQ to create "Cow" switches
 */

#include "z_en_wonder_item.h"
#include "vt.h"

#define FLAGS 0

void EnWonderItem_Init(Actor* thisx, GlobalContext* globalCtx);
void EnWonderItem_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnWonderItem_Update(Actor* thisx, GlobalContext* globalCtx);

void EnWonderItem_MultitagFree(EnWonderItem* thisv, GlobalContext* globalCtx);
void EnWonderItem_ProximityDrop(EnWonderItem* thisv, GlobalContext* globalCtx);
void EnWonderItem_InteractSwitch(EnWonderItem* thisv, GlobalContext* globalCtx);
void EnWonderItem_ProximitySwitch(EnWonderItem* thisv, GlobalContext* globalCtx);
void EnWonderItem_MultitagOrdered(EnWonderItem* thisv, GlobalContext* globalCtx);
void EnWonderItem_BombSoldier(EnWonderItem* thisv, GlobalContext* globalCtx);
void EnWonderItem_RollDrop(EnWonderItem* thisv, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 20, 30, 0, { 0, 0, 0 } },
};

const ActorInit En_Wonder_Item_InitVars = {
    ACTOR_EN_WONDER_ITEM,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnWonderItem),
    (ActorFunc)EnWonderItem_Init,
    (ActorFunc)EnWonderItem_Destroy,
    (ActorFunc)EnWonderItem_Update,
    NULL,
    NULL,
};

static Vec3f sTagPointsFree[9];
static Vec3f sTagPointsOrdered[9];

void EnWonderItem_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnWonderItem* thisv = (EnWonderItem*)thisx;

    if ((thisv->collider.dim.radius != 0) || (thisv->collider.dim.height != 0)) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void EnWonderItem_DropCollectible(EnWonderItem* thisv, GlobalContext* globalCtx, s32 autoCollect) {
    static s16 dropTable[] = {
        ITEM00_NUTS,        ITEM00_HEART_PIECE,  ITEM00_MAGIC_LARGE,   ITEM00_MAGIC_SMALL,
        ITEM00_HEART,       ITEM00_ARROWS_SMALL, ITEM00_ARROWS_MEDIUM, ITEM00_ARROWS_LARGE,
        ITEM00_RUPEE_GREEN, ITEM00_RUPEE_BLUE,   ITEM00_RUPEE_RED,     ITEM00_FLEXIBLE,
    };
    s32 i;
    s32 randomDrop;

    func_80078884(NA_SE_SY_GET_ITEM);

    if (thisv->dropCount == 0) {
        thisv->dropCount++;
    }
    for (i = thisv->dropCount; i > 0; i--) {
        if (thisv->itemDrop < WONDERITEM_DROP_RANDOM) {
            if ((thisv->itemDrop == WONDERITEM_DROP_FLEXIBLE) || !autoCollect) {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos, dropTable[thisv->itemDrop]);
            } else {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos, dropTable[thisv->itemDrop] | 0x8000);
            }
        } else {
            randomDrop = thisv->itemDrop - WONDERITEM_DROP_RANDOM;
            if (!autoCollect) {
                Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, randomDrop);
            } else {
                Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, randomDrop | 0x8000);
            }
        }
    }
    if (thisv->switchFlag >= 0) {
        Flags_SetSwitch(globalCtx, thisv->switchFlag);
    }
    Actor_Kill(&thisv->actor);
}

void EnWonderItem_Init(Actor* thisx, GlobalContext* globalCtx) {
    static u32 collisionTypes[] = {
        0x00000702 /* sword slash */, 0x0001F820 /* arrow */,     0x00000040 /* hammer */,   0x00000008 /* bomb */,
        0x00000004 /* slingshot */,   0x00000010 /* boomerang */, 0x00000080 /* hookshot */,
    };
    s32 pad;
    s16 colTypeIndex;
    EnWonderItem* thisv = (EnWonderItem*)thisx;
    s16 rotZover10;
    s16 tagIndex;

    osSyncPrintf("\n\n");
    // "Mysterious mystery, very mysterious"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 不思議不思議まか不思議 \t   ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.params);
    thisv->actor.flags &= ~ACTOR_FLAG_0;

    thisv->wonderMode = (thisv->actor.params >> 0xB) & 0x1F;
    thisv->itemDrop = (thisv->actor.params >> 6) & 0x1F;
    thisv->switchFlag = thisv->actor.params & 0x3F;
    if (thisv->switchFlag == 0x3F) {
        thisv->switchFlag = -1;
    }
    thisv->actor.targetMode = 1;
    if ((thisv->switchFlag >= 0) && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Ｙｏｕ ａｒｅ Ｓｈｏｃｋ！  ☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
        Actor_Kill(&thisv->actor);
        return;
    }
    switch (thisv->wonderMode) {
        case WONDERITEM_MULTITAG_FREE:
            thisv->numTagPoints = thisv->actor.world.rot.z & 0xF;
            rotZover10 = 0;
            if (thisv->actor.world.rot.z >= 10) {
                rotZover10 = thisv->actor.world.rot.z / 10;
                thisv->timerMod = rotZover10 * 20;
            }
            thisv->numTagPoints = thisv->actor.world.rot.z - rotZover10 * 10;
            // i.e timerMod = rot.z / 10 seconds, numTagPoints = rot.z % 10
            thisv->updateFunc = EnWonderItem_MultitagFree;
            break;
        case WONDERITEM_TAG_POINT_FREE:
            tagIndex = thisv->actor.world.rot.z & 0xFF;
            sTagPointsFree[tagIndex] = thisv->actor.world.pos;
            Actor_Kill(&thisv->actor);
            break;
        case WONDERITEM_PROXIMITY_DROP:
            thisv->dropCount = thisv->actor.world.rot.z & 0xFF;
            thisv->updateFunc = EnWonderItem_ProximityDrop;
            break;
        case WONDERITEM_INTERACT_SWITCH:
            colTypeIndex = thisv->actor.world.rot.z & 0xFF;
            Collider_InitCylinder(globalCtx, &thisv->collider);
            Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
            thisv->collider.info.bumper.dmgFlags = collisionTypes[colTypeIndex];
            thisv->collider.dim.radius = 20;
            thisv->collider.dim.height = 30;
            thisv->updateFunc = EnWonderItem_InteractSwitch;
            break;
        case WONDERITEM_UNUSED:
            break;
        case WONDERITEM_MULTITAG_ORDERED:
            thisv->numTagPoints = thisv->actor.world.rot.z & 0xF;
            rotZover10 = 0;
            if (thisv->actor.world.rot.z >= 10) {
                rotZover10 = thisv->actor.world.rot.z / 10;
                thisv->timerMod = rotZover10 * 20;
            }
            thisv->numTagPoints = thisv->actor.world.rot.z - rotZover10 * 10;
            // i.e timerMod = rot.z / 10 seconds, numTagPoints = rot.z % 10
            thisv->updateFunc = EnWonderItem_MultitagOrdered;
            break;
        case WONDERITEM_TAG_POINT_ORDERED:
            tagIndex = thisv->actor.world.rot.z & 0xFF;
            sTagPointsOrdered[tagIndex] = thisv->actor.world.pos;
            Actor_Kill(&thisv->actor);
            break;
        case WONDERITEM_PROXIMITY_SWITCH:
            thisv->updateFunc = EnWonderItem_ProximitySwitch;
            break;
        case WONDERITEM_BOMB_SOLDIER:
            Collider_InitCylinder(globalCtx, &thisv->collider);
            Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
            thisv->collider.info.bumper.dmgFlags = 0x00000004; // slingshot
            thisv->unkPos = thisv->actor.world.pos;
            thisv->collider.dim.radius = 35;
            thisv->collider.dim.height = 75;
            thisv->updateFunc = EnWonderItem_BombSoldier;
            break;
        case WONDERITEM_ROLL_DROP:
            thisv->dropCount = thisv->actor.world.rot.z & 0xFF;
            thisv->updateFunc = EnWonderItem_RollDrop;
            break;
        default:
            Actor_Kill(&thisv->actor);
            break;
    }
}

void EnWonderItem_MultitagFree(EnWonderItem* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 prevTagFlags = thisv->tagFlags;
    s32 i;
    s32 mask;

    for (i = 0, mask = 1; i < thisv->numTagPoints; i++, mask <<= 1) {
        if (!(prevTagFlags & mask)) {
            f32 dx = player->actor.world.pos.x - sTagPointsFree[i].x;
            f32 dy = player->actor.world.pos.y - sTagPointsFree[i].y;
            f32 dz = player->actor.world.pos.z - sTagPointsFree[i].z;

            if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 50.0f) {
                thisv->tagFlags |= mask;
                thisv->tagCount++;
                thisv->timer = thisv->timerMod + 81;
                return;
            }
            if (BREG(0) != 0) {
                DebugDisplay_AddObject(sTagPointsFree[i].x, sTagPointsFree[i].y, sTagPointsFree[i].z,
                                       thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 0, 255, 0, 255, 4, globalCtx->state.gfxCtx);
            }
        }
    }
    if (thisv->timer == 1) {
        Actor_Kill(&thisv->actor);
        return;
    }
    if (thisv->tagCount == thisv->numTagPoints) {
        if (thisv->switchFlag >= 0) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
        }
        EnWonderItem_DropCollectible(thisv, globalCtx, true);
    }
}

void EnWonderItem_ProximityDrop(EnWonderItem* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->actor.xzDistToPlayer < 50.0f) && (fabsf(thisv->actor.world.pos.y - player->actor.world.pos.y) < 30.0f)) {
        EnWonderItem_DropCollectible(thisv, globalCtx, true);
    }
}

void EnWonderItem_InteractSwitch(EnWonderItem* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        EnWonderItem_DropCollectible(thisv, globalCtx, false);
    }
}

void EnWonderItem_ProximitySwitch(EnWonderItem* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->actor.xzDistToPlayer < 50.0f) && (fabsf(thisv->actor.world.pos.y - player->actor.world.pos.y) < 30.0f)) {
        if (thisv->switchFlag >= 0) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
        }
        Actor_Kill(&thisv->actor);
    }
}

void EnWonderItem_MultitagOrdered(EnWonderItem* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 prevTagFlags = thisv->tagFlags;
    s32 i;
    s32 mask;

    for (i = 0, mask = 1; i < thisv->numTagPoints; i++, mask <<= 1) {
        if (!(prevTagFlags & mask)) {
            f32 dx = player->actor.world.pos.x - sTagPointsOrdered[i].x;
            f32 dy = player->actor.world.pos.y - sTagPointsOrdered[i].y;
            f32 dz = player->actor.world.pos.z - sTagPointsOrdered[i].z;

            if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 50.0f) {
                if (prevTagFlags & mask) {
                    return;
                } else if (i == thisv->nextTag) {
                    thisv->tagFlags |= mask;
                    thisv->tagCount++;
                    thisv->nextTag++;
                    thisv->timer = thisv->timerMod + 81;
                    return;
                } else {
                    Actor_Kill(&thisv->actor);
                    return;
                }
            } else if (BREG(0) != 0) {
                DebugDisplay_AddObject(sTagPointsOrdered[i].x, sTagPointsOrdered[i].y, sTagPointsOrdered[i].z,
                                       thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 0, 0, 255, 255, 4, globalCtx->state.gfxCtx);
            }
        }
    }
    if (thisv->timer == 1) {
        Actor_Kill(&thisv->actor);
        return;
    }
    if (thisv->tagCount == thisv->numTagPoints) {
        EnWonderItem_DropCollectible(thisv, globalCtx, true);
    }
}

void EnWonderItem_BombSoldier(EnWonderItem* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_HEISHI2, thisv->actor.world.pos.x,
                        thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, thisv->actor.yawTowardsPlayer, 0,
                        9) != NULL) {
            // "Careless soldier spawned"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ うっかり兵セット完了 ☆☆☆☆☆ \n" VT_RST);
        }
        if (thisv->switchFlag >= 0) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
        }
        Actor_Kill(&thisv->actor);
    }
}

void EnWonderItem_RollDrop(EnWonderItem* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->actor.xzDistToPlayer < 50.0f) && (player->invincibilityTimer < 0) &&
        (fabsf(thisv->actor.world.pos.y - player->actor.world.pos.y) < 30.0f)) {
        EnWonderItem_DropCollectible(thisv, globalCtx, true);
    }
}

void EnWonderItem_Update(Actor* thisx, GlobalContext* globalCtx) {
    static s16 debugArrowColors[] = {
        255, 255, 0,   255, 0,   255, 0,   255, 255, 255, 0,   0, 0, 255, 0,   0, 0, 255, 128, 128,
        128, 128, 128, 0,   128, 0,   128, 0,   128, 0,   128, 0, 0, 0,   128, 0, 0, 0,   128,
    }; // These seem to be mistyped. Logically they should be s16[13][3] and be indexed as [colorIndex][i]
    s32 pad;
    EnWonderItem* thisv = (EnWonderItem*)thisx;
    s32 colorIndex;

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    thisv->updateFunc(thisv, globalCtx);

    if (thisv->wonderMode == WONDERITEM_UNUSED) {
        Actor_SetFocus(&thisv->actor, thisv->unkHeight);
    }
    if ((thisv->wonderMode == WONDERITEM_INTERACT_SWITCH) || (thisv->wonderMode == WONDERITEM_BOMB_SOLDIER)) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    colorIndex = thisv->wonderMode;
    if (thisv->wonderMode > 12) {
        colorIndex = 0;
    }
    if (BREG(0) != 0) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                               thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, debugArrowColors[colorIndex], debugArrowColors[colorIndex + 1],
                               debugArrowColors[colorIndex + 2], 255, 4, globalCtx->state.gfxCtx);
    }
}
