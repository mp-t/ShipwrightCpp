/*
 * File: z_en_kanban.c
 * Overlay: ovl_En_Kanban
 * Description: Cuttable square sign
 */

#include "z_en_kanban.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_kanban/object_kanban.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

#define PART_UPPER_LEFT (1 << 0)
#define PART_LEFT_UPPER (1 << 1)
#define PART_LEFT_LOWER (1 << 2)
#define PART_RIGHT_UPPER (1 << 3)
#define PART_RIGHT_LOWER (1 << 4)
#define PART_LOWER_LEFT (1 << 5)
#define PART_UPPER_RIGHT (1 << 6)
#define PART_LOWER_RIGHT (1 << 7)
#define PART_POST_UPPER (1 << 8)
#define PART_POST_LOWER (1 << 9)
#define PART_POST_STAND (1 << 10)
#define LEFT_HALF (PART_UPPER_LEFT | PART_LEFT_UPPER | PART_LEFT_LOWER | PART_LOWER_LEFT)
#define RIGHT_HALF (PART_UPPER_RIGHT | PART_RIGHT_UPPER | PART_RIGHT_LOWER | PART_LOWER_RIGHT)
#define UPPER_HALF (PART_POST_UPPER | PART_UPPER_RIGHT | PART_RIGHT_UPPER | PART_UPPER_LEFT | PART_LEFT_UPPER)
#define UPPERLEFT_HALF (PART_POST_UPPER | PART_UPPER_RIGHT | PART_LEFT_LOWER | PART_UPPER_LEFT | PART_LEFT_UPPER)
#define UPPERRIGHT_HALF (PART_POST_UPPER | PART_UPPER_RIGHT | PART_RIGHT_UPPER | PART_UPPER_LEFT | PART_RIGHT_LOWER)
#define ALL_PARTS (LEFT_HALF | RIGHT_HALF | PART_POST_UPPER | PART_POST_LOWER)

typedef enum {
    ENKANBAN_SIGN,
    ENKANBAN_AIR,
    ENKANBAN_UNUSED,
    ENKANBAN_GROUND,
    ENKANBAN_WATER,
    ENKANBAN_REPAIR
} EnKanbanActionState;

typedef enum {
    PIECE_WHOLE_SIGN,
    PIECE_UPPER_HALF,
    PIECE_LOWER_HALF,
    PIECE_RIGHT_HALF,
    PIECE_LEFT_HALF,
    PIECE_2ND_QUAD,
    PIECE_1ST_QUAD,
    PIECE_3RD_QUAD,
    PIECE_4TH_QUAD,
    PIECE_UPPER_LEFT,
    PIECE_LEFT_UPPER,
    PIECE_LEFT_LOWER,
    PIECE_LOWER_LEFT,
    PIECE_UPPER_RIGHT,
    PIECE_RIGHT_UPPER,
    PIECE_RIGHT_LOWER,
    PIECE_LOWER_RIGHT,
    PIECE_POST_UPPER,
    PIECE_POST_LOWER,
    PIECE_OTHER = 100
} EnKanbanPiece;

typedef enum {
    CUT_POST,
    CUT_VERT_L,
    CUT_HORIZ,
    CUT_DIAG_L, // lower left to upper right
    CUT_DIAG_R, // upper left to lower right
    CUT_VERT_R
} EnKanbanCutType;

void EnKanban_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKanban_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKanban_Update(Actor* thisx, GlobalContext* globalCtx);
void EnKanban_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit En_Kanban_InitVars = {
    ACTOR_EN_KANBAN,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_KANBAN,
    sizeof(EnKanban),
    (ActorFunc)EnKanban_Init,
    (ActorFunc)EnKanban_Destroy,
    (ActorFunc)EnKanban_Update,
    (ActorFunc)EnKanban_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 50, 5, { 0, 0, 0 } },
};

static u16 sPartFlags[] = {
    PART_UPPER_LEFT,  PART_LEFT_UPPER,  PART_LEFT_LOWER, PART_RIGHT_UPPER, PART_RIGHT_LOWER, PART_LOWER_LEFT,
    PART_UPPER_RIGHT, PART_LOWER_RIGHT, PART_POST_UPPER, PART_POST_LOWER,  PART_POST_STAND,
};

static Vec3f sPieceOffsets[] = {
    /* WHOLE_SIGN   */ { 0.0f, 44.0f, 0.0f },
    /* UPPER_HALF   */ { 0.0f, 50.0f, 0.0f },
    /* LOWER_HALF   */ { 0.0f, 38.0f, 0.0f },
    /* RIGHT_HALF  */ { 10.0f, 44.0f, 0.0f },
    /* LEFT_HALF  */ { -10.0f, 44.0f, 0.0f },
    /* 2ND_QUAD   */ { -10.0f, 50.0f, 0.0f },
    /* 1ST_QUAD    */ { 10.0f, 50.0f, 0.0f },
    /* 3RD_QUAD   */ { -10.0f, 38.0f, 0.0f },
    /* 4TH_QUAD    */ { 10.0f, 38.0f, 0.0f },
    /* UPPER_LEFT  */ { -7.5f, 51.0f, 0.0f },
    /* LEFT_UPPER */ { -12.5f, 48.0f, 0.0f },
    /* LEFT_LOWER */ { -12.5f, 40.0f, 0.0f },
    /* LOWER_LEFT  */ { -7.5f, 37.0f, 0.0f },
    /* UPPER_RIGHT  */ { 7.5f, 51.0f, 0.0f },
    /* RIGHT_UPPER */ { 12.5f, 48.0f, 0.0f },
    /* RIGHT_LOWER */ { 12.5f, 40.0f, 0.0f },
    /* LOWER_RIGHT  */ { 7.5f, 37.0f, 0.0f },
    /* POST_UPPER   */ { 0.0f, 50.0f, 0.0f },
    /* POST_LOWER   */ { 0.0f, 38.0f, 0.0f },
};

static Vec3f sPieceSizes[] = {
    /* WHOLE_SIGN */ { 1500.0f, 1000.0f, 0.0f },
    /* UPPER_HALF */ { 1500.0f, 500.0f, 0.0f },
    /* LOWER_HALF */ { 1500.0f, 500.0f, 0.0f },
    /* RIGHT_HALF  */ { 700.0f, 1000.0f, 0.0f },
    /* LEFT_HALF   */ { 700.0f, 1000.0f, 0.0f },
    /* 2ND_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* 1ST_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* 3RD_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* 4TH_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* UPPER_LEFT  */ { 700.0f, 500.0f, 0.0f },
    /* LEFT_UPPER  */ { 700.0f, 500.0f, 0.0f },
    /* LEFT_LOWER  */ { 700.0f, 500.0f, 0.0f },
    /* LOWER_LEFT  */ { 700.0f, 500.0f, 0.0f },
    /* UPPER_RIGHT */ { 700.0f, 500.0f, 0.0f },
    /* RIGHT_UPPER */ { 700.0f, 500.0f, 0.0f },
    /* RIGHT_LOWER */ { 700.0f, 500.0f, 0.0f },
    /* LOWER_RIGHT */ { 700.0f, 500.0f, 0.0f },
    /* POST_UPPER  */ { 200.0f, 500.0f, 0.0f },
    /* POST_LOWER  */ { 200.0f, 500.0f, 0.0f },
};

static u8 sCutTypes[] = {
    /* 1H_OVER     */ CUT_VERT_L, /* 2H_OVER     */ CUT_VERT_L,
    /* 1H_COMBO    */ CUT_DIAG_R, /* 2H_COMBO    */ CUT_DIAG_R,
    /* 1H_LEFT     */ CUT_HORIZ,  /* 2H_LEFT     */ CUT_HORIZ,
    /* 1H_COMBO    */ CUT_HORIZ,  /* 2H_COMBO    */ CUT_HORIZ,
    /* 1H_RIGHT    */ CUT_HORIZ,  /* 2H_RIGHT    */ CUT_HORIZ,
    /* 1H_COMBO    */ CUT_HORIZ,  /* 2H_COMBO    */ CUT_HORIZ,
    /* 1H_STAB     */ CUT_POST,   /* 2H_STAB     */ CUT_POST,
    /* 1H_COMBO    */ CUT_POST,   /* 2H_COMBO    */ CUT_POST,
    /* FLIP_START  */ CUT_VERT_L, /* JUMP_START  */ CUT_VERT_L,
    /* FLIP_END    */ CUT_VERT_L, /* JUMP_END    */ CUT_VERT_L,
    /* BACK_LEFT   */ CUT_HORIZ,  /* BACK_RIGHT  */ CUT_HORIZ,
    /* OVER_HAMMER */ CUT_POST,   /* SIDE_HAMMER */ CUT_POST,
    /* 1H_SPIN_ATK */ CUT_POST,   /* 2H_SPIN_ATK */ CUT_POST,
    /* 1H_BIG_SPIN */ CUT_POST,   /* 2H_BIG_SPIN */ CUT_POST,
};

static u16 sCutFlags[] = {
    /* CUT_POST   */ ALL_PARTS,       /* CUT_VERT_L */ LEFT_HALF,
    /* CUT_HORIZ  */ UPPER_HALF,      /* CUT_DIAG_L */ UPPERLEFT_HALF,
    /* CUT_DIAG_R */ UPPERRIGHT_HALF, /* CUT_VERT_R */ RIGHT_HALF,
};

void EnKanban_SetFloorRot(EnKanban* thisv) {
    if (thisv->actor.floorPoly != NULL) {
        f32 nx = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.x);
        f32 ny = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.y);
        f32 nz = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.z);

        thisv->floorRot.x = -Math_FAtan2F(-nz * ny, 1.0f);
        thisv->floorRot.z = Math_FAtan2F(-nx * ny, 1.0f);
    }
}

void EnKanban_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKanban* thisv = (EnKanban*)thisx;

    Actor_SetScale(&thisv->actor, 0.01f);
    if (thisv->actor.params != ENKANBAN_PIECE) {
        thisv->actor.targetMode = 0;
        thisv->actor.flags |= ACTOR_FLAG_0;
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        osSyncPrintf("KANBAN ARG    %x\n", thisv->actor.params);
        if (thisv->actor.params == ENKANBAN_FISHING) {
            if (LINK_IS_CHILD) {
                thisv->actor.textId = 0x409D;
            } else {
                thisv->actor.textId = 0x4090;
            }
        } else {
            thisv->actor.textId = thisv->actor.params | 0x300;
        }
        thisv->bounceX = 1;
        thisv->partFlags = 0xFFFF;
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 50.0f, 4);
        EnKanban_SetFloorRot(thisv);
        if (LINK_IS_CHILD) {
            thisv->actor.world.pos.y -= 15.0f;
        }
    }
}

void EnKanban_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnKanban* thisv = (EnKanban*)thisx;

    if (thisv->actionState == ENKANBAN_SIGN) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void EnKanban_Message(EnKanban* thisv, GlobalContext* globalCtx) {
    if (!thisv->msgFlag) {
        if (thisv->msgTimer == 0) {
            if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) < 0x2800) {
                if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
                    thisv->msgFlag = true;
                } else {
                    func_8002F2CC(&thisv->actor, globalCtx, 68.0f);
                }
            }
        } else {
            thisv->msgTimer--;
        }
    } else {
        if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
            thisv->msgFlag = false;
            thisv->msgTimer = 20;
        }
    }
}

void EnKanban_Update(Actor* thisx, GlobalContext* globalCtx2) {
    u8 bounced = false;
    GlobalContext* globalCtx = globalCtx2;
    EnKanban* thisv = (EnKanban*)thisx;
    EnKanban* signpost;
    EnKanban* piece;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f offset;

    thisv->frameCount++;
    switch (thisv->actionState) {
        case ENKANBAN_SIGN:
            if (thisv->invincibilityTimer != 0) {
                thisv->invincibilityTimer--;
            }
            if (thisv->zTargetTimer != 0) {
                thisv->zTargetTimer--;
            }
            if (thisv->zTargetTimer == 1) {
                thisv->actor.flags &= ~ACTOR_FLAG_0;
            }
            if (thisv->partFlags == 0xFFFF) {
                EnKanban_Message(thisv, globalCtx);
            }
            if ((thisv->invincibilityTimer == 0) && (thisv->collider.base.acFlags & AC_HIT)) {
                thisv->collider.base.acFlags &= ~AC_HIT;
                thisv->invincibilityTimer = 6;
                piece = (EnKanban*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_KANBAN,
                                                      thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                                      thisv->actor.world.pos.z, thisv->actor.shape.rot.x,
                                                      thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, ENKANBAN_PIECE);
                if (piece != NULL) {
                    ColliderInfo* hitItem = thisv->collider.info.acHitInfo;
                    s16 yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
                    u8 i;

                    if (hitItem->toucher.dmgFlags & 0x700) {
                        thisv->cutType = sCutTypes[player->swordAnimation];
                    } else {
                        thisv->cutType = CUT_POST;
                    }
                    if (ABS(yawDiff) > 0x4000) {
                        if (thisv->cutType == CUT_DIAG_R) {
                            thisv->cutType = CUT_DIAG_L;
                        } else if (thisv->cutType == CUT_VERT_L) {
                            thisv->cutType = CUT_VERT_R;
                        }
                    }
                    piece->partFlags = sCutFlags[thisv->cutType] & thisv->partFlags;
                    if (piece->partFlags == 0) {
                        Actor_Kill(&piece->actor);
                        return;
                    }
                    piece->partCount = 0;
                    for (i = 0; i < ARRAY_COUNT(sPartFlags); i++) {
                        if (sPartFlags[i] & piece->partFlags) {
                            piece->partCount++;
                        }
                    }
                    thisv->partFlags &= ~sCutFlags[thisv->cutType];
                    if (!(thisv->partFlags & ALL_PARTS)) {
                        thisv->zTargetTimer = 10;
                    }
                    if ((piece->partFlags & PART_UPPER_LEFT) && (piece->partFlags & PART_LOWER_RIGHT)) {
                        piece->pieceType = PIECE_WHOLE_SIGN;
                    } else if ((piece->partFlags & PART_LEFT_UPPER) && (piece->partFlags & PART_RIGHT_UPPER)) {
                        piece->pieceType = PIECE_UPPER_HALF;
                    } else if ((piece->partFlags & PART_LEFT_LOWER) && (piece->partFlags & PART_RIGHT_LOWER)) {
                        piece->pieceType = PIECE_LOWER_HALF;
                    } else if ((piece->partFlags & PART_UPPER_RIGHT) && (piece->partFlags & PART_LOWER_RIGHT)) {
                        piece->pieceType = PIECE_RIGHT_HALF;
                    } else if ((piece->partFlags & PART_UPPER_LEFT) && (piece->partFlags & PART_LOWER_LEFT)) {
                        piece->pieceType = PIECE_LEFT_HALF;
                    } else if ((piece->partFlags & PART_UPPER_LEFT) && (piece->partFlags & PART_LEFT_UPPER)) {
                        piece->pieceType = PIECE_2ND_QUAD;
                    } else if ((piece->partFlags & PART_UPPER_RIGHT) && (piece->partFlags & PART_RIGHT_UPPER)) {
                        piece->pieceType = PIECE_1ST_QUAD;
                    } else if ((piece->partFlags & PART_LEFT_LOWER) && (piece->partFlags & PART_LOWER_LEFT)) {
                        piece->pieceType = PIECE_3RD_QUAD;
                    } else if ((piece->partFlags & PART_RIGHT_LOWER) && (piece->partFlags & PART_LOWER_RIGHT)) {
                        piece->pieceType = PIECE_4TH_QUAD;
                    } else if (piece->partFlags & PART_UPPER_LEFT) {
                        piece->pieceType = PIECE_UPPER_LEFT;
                    } else if (piece->partFlags & PART_LEFT_UPPER) {
                        piece->pieceType = PIECE_LEFT_UPPER;
                    } else if (piece->partFlags & PART_LEFT_LOWER) {
                        piece->pieceType = PIECE_LEFT_LOWER;
                    } else if (piece->partFlags & PART_LOWER_LEFT) {
                        piece->pieceType = PIECE_LOWER_LEFT;
                    } else if (piece->partFlags & PART_UPPER_RIGHT) {
                        piece->pieceType = PIECE_UPPER_RIGHT;
                    } else if (piece->partFlags & PART_RIGHT_UPPER) {
                        piece->pieceType = PIECE_RIGHT_UPPER;
                    } else if (piece->partFlags & PART_RIGHT_LOWER) {
                        piece->pieceType = PIECE_RIGHT_LOWER;
                    } else if (piece->partFlags & PART_LOWER_RIGHT) {
                        piece->pieceType = PIECE_LOWER_RIGHT;
                    } else if (piece->partFlags & PART_POST_UPPER) {
                        piece->pieceType = PIECE_POST_UPPER;
                    } else if (piece->partFlags & PART_POST_LOWER) {
                        piece->pieceType = PIECE_POST_LOWER;
                    } else {
                        piece->pieceType = PIECE_OTHER;
                    }
                    if (piece->pieceType == 100) {
                        piece->pieceType = PIECE_WHOLE_SIGN;
                    }

                    Matrix_RotateY((thisv->actor.shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_NEW);
                    Matrix_MultVec3f(&sPieceOffsets[piece->pieceType], &offset);
                    piece->actor.world.pos.x += offset.x;
                    piece->actor.world.pos.y += offset.y;
                    piece->actor.world.pos.z += offset.z;
                    piece->offset.x = -sPieceOffsets[piece->pieceType].x / thisv->actor.scale.x;
                    piece->offset.y = -sPieceOffsets[piece->pieceType].y / thisv->actor.scale.x;
                    piece->offset.z = -sPieceOffsets[piece->pieceType].z / thisv->actor.scale.x;
                    piece->pieceWidth = sPieceSizes[piece->pieceType].x;
                    piece->pieceHeight = sPieceSizes[piece->pieceType].y;
                    piece->actor.gravity = -1.0f;
                    piece->actionState = ENKANBAN_AIR;
                    piece->actor.world.rot.y = (s16)Rand_CenteredFloat(0x3000) + thisv->actor.yawTowardsPlayer + 0x8000;
                    piece->actor.velocity.y = Rand_ZeroFloat(2.0f) + 3.0f;
                    piece->actor.speedXZ = Rand_ZeroFloat(2.0f) + 3.0f;
                    if (piece->partCount >= 4) {
                        piece->bounceX = (s16)Rand_ZeroFloat(10.0f) + 6;
                        piece->bounceZ = (s16)Rand_ZeroFloat(10.0f) + 6;
                    } else {
                        piece->bounceX = (s16)Rand_ZeroFloat(7.0f) + 3;
                        piece->bounceZ = (s16)Rand_ZeroFloat(7.0f) + 3;
                    }
                    piece->spinVel.y = Rand_CenteredFloat(0x1800);
                    if (Rand_ZeroOne() < 0.5f) {
                        piece->direction = 1;
                    } else {
                        piece->direction = -1;
                    }
                    piece->airTimer = 100;
                    piece->actor.flags &= ~ACTOR_FLAG_0;
                    piece->actor.flags |= ACTOR_FLAG_25;
                    thisv->cutMarkTimer = 5;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_SWORD_STRIKE);
                }
            }
            thisv->actor.focus.pos = thisv->actor.world.pos;
            thisv->actor.focus.pos.y += 44.0f;
            Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            if (thisv->actor.xzDistToPlayer > 500.0f) {
                thisv->actor.flags |= ACTOR_FLAG_0;
                thisv->partFlags = 0xFFFF;
            }
            if (thisv->cutMarkTimer != 0) {
                if (thisv->cutMarkTimer >= 5) {
                    thisv->cutMarkAlpha += 255;
                    if (thisv->cutMarkAlpha > 255) {
                        thisv->cutMarkAlpha = 255;
                    }
                } else {
                    thisv->cutMarkAlpha -= 65;
                    if (thisv->cutMarkAlpha < 0) {
                        thisv->cutMarkAlpha = 0;
                    }
                }
                thisv->cutMarkTimer--;
            }
            break;
        case ENKANBAN_AIR:
        case ENKANBAN_UNUSED: {
            u16 tempBgFlags;
            f32 tempX;
            f32 tempY;
            f32 tempZ;
            f32 tempYDistToWater;
            u8 onGround;

            Actor_MoveForward(&thisv->actor);
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 30.0f, 30.0f, 50.0f, 5);

            tempX = thisv->actor.world.pos.x;
            tempY = thisv->actor.world.pos.y;
            tempZ = thisv->actor.world.pos.z;
            tempBgFlags = thisv->actor.bgCheckFlags;
            tempYDistToWater = thisv->actor.yDistToWater;

            thisv->actor.world.pos.z += ((thisv->actor.world.pos.y - thisv->actor.floorHeight) * -50.0f) / 100.0f;
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 50.0f, 4);
            EnKanban_SetFloorRot(thisv);

            thisv->actor.world.pos.x = tempX;
            thisv->actor.world.pos.y = tempY;
            thisv->actor.world.pos.z = tempZ;
            thisv->actor.bgCheckFlags = tempBgFlags;
            thisv->actor.yDistToWater = tempYDistToWater;

            osSyncPrintf(VT_RST);
            onGround = (thisv->actor.bgCheckFlags & 1);
            if (thisv->spinXFlag) {
                thisv->spinRot.x += thisv->spinVel.x;
                thisv->spinVel.x -= 0x800;
                if ((thisv->spinRot.x <= 0) && onGround) {
                    thisv->spinRot.x = 0;
                    thisv->spinVel.x = 0;
                }
            } else {
                thisv->spinRot.x -= thisv->spinVel.x;
                thisv->spinVel.x -= 0x800;
                if ((thisv->spinRot.x >= 0) && onGround) {
                    thisv->spinRot.x = 0;
                    thisv->spinVel.x = 0;
                }
            }
            if (thisv->spinVel.x < -0xC00) {
                thisv->spinVel.x = -0xC00;
            }
            if (thisv->spinZFlag) {
                thisv->spinRot.z += thisv->spinVel.z;
                thisv->spinVel.z -= 0x800;
                if ((thisv->spinRot.z <= 0) && onGround) {
                    thisv->spinRot.z = 0;
                    thisv->spinVel.z = 0;
                }
            } else {
                thisv->spinRot.z -= thisv->spinVel.z;
                thisv->spinVel.z -= 0x800;
                if ((thisv->spinRot.z >= 0) && onGround) {
                    thisv->spinRot.z = 0;
                    thisv->spinVel.z = 0;
                }
            }
            if (thisv->spinVel.z < -0xC00) {
                thisv->spinVel.z = -0xC00;
            }
            if (thisv->actor.bgCheckFlags & 8) {
                thisv->actor.speedXZ *= -0.5f;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_WOODPLATE_BOUND);
            }
            if (thisv->actor.bgCheckFlags & 0x40) {
                thisv->actionState = ENKANBAN_WATER;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_BOMB_DROP_WATER);
                thisv->bounceX = thisv->bounceZ = 0;
                thisv->actor.world.pos.y += thisv->actor.yDistToWater;
                EffectSsGSplash_Spawn(globalCtx, &thisv->actor.world.pos, NULL, NULL, 0, (thisv->partCount * 20) + 300);
                EffectSsGRipple_Spawn(globalCtx, &thisv->actor.world.pos, 150, 650, 0);
                EffectSsGRipple_Spawn(globalCtx, &thisv->actor.world.pos, 300, 800, 5);
                thisv->actor.velocity.y = 0.0f;
                thisv->actor.gravity = 0.0f;
                osSyncPrintf(" WAT  Y  = %f\n", thisv->actor.yDistToWater);
                osSyncPrintf(" POS  Y  = %f\n", thisv->actor.world.pos.y);
                osSyncPrintf(" GROUND Y  = %f\n", thisv->actor.floorHeight);
                break;
            }

            if (onGround) {
                if (thisv->bounceCount <= 0) {
                    thisv->bounceCount++;
                    thisv->actor.velocity.y *= -0.3f;
                    thisv->actor.world.rot.y += (s16)Rand_CenteredFloat(16384.0f);
                } else {
                    thisv->actor.velocity.y = 0.0f;
                }
                thisv->actor.speedXZ *= 0.7f;
                if ((thisv->spinRot.x == 0) && (thisv->bounceX != 0)) {
                    thisv->spinVel.x = thisv->bounceX * 0x200;
                    if (thisv->bounceX != 0) {
                        thisv->bounceX -= 5;
                        if (thisv->bounceX <= 0) {
                            thisv->bounceX = 0;
                        }
                    }
                    if (Rand_ZeroOne() < 0.5f) {
                        thisv->spinXFlag = true;
                    } else {
                        thisv->spinXFlag = false;
                    }
                    bounced = true;
                }
                if ((thisv->spinRot.z == 0) && (thisv->bounceZ != 0)) {
                    thisv->spinVel.z = thisv->bounceZ * 0x200;
                    if (thisv->bounceZ != 0) {
                        thisv->bounceZ -= 5;
                        if (thisv->bounceZ <= 0) {
                            thisv->bounceZ = 0;
                        }
                    }
                    if (Rand_ZeroOne() < 0.5f) {
                        thisv->spinZFlag = true;
                    } else {
                        thisv->spinZFlag = false;
                    }
                    bounced = true;
                }
                Math_ApproachS(&thisv->actor.shape.rot.x, thisv->direction * 0x4000, 1, 0x2000);
            } else {
                thisv->actor.shape.rot.y += thisv->spinVel.y;
                thisv->actor.shape.rot.x += thisv->direction * 0x7D0;
            }
            if (bounced) {
                s16 dustCount;
                s16 j;
                Vec3f velocity = { 0.0f, 0.0f, 0.0f };
                Vec3f accel;
                Vec3f pos;

                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_WOODPLATE_BOUND);
                accel.x = 0.0f;
                accel.y = 0.1f;
                accel.z = 0.0f;
                pos.y = thisv->actor.floorHeight + 3.0f;
                dustCount = thisv->partCount * 0.5f;
                for (j = 0; j < dustCount + 3; j++) {
                    pos.x = thisv->actor.world.pos.x + Rand_CenteredFloat((thisv->partCount * 0.5f) + 20.0f);
                    pos.z = thisv->actor.world.pos.z + Rand_CenteredFloat((thisv->partCount * 0.5f) + 20.0f);
                    func_800286CC(globalCtx, &pos, &velocity, &accel, 100, 5);
                }
            }
            if (DECR(thisv->airTimer) == 0) {
                thisv->actionState = ENKANBAN_GROUND;
            }
        }
        case ENKANBAN_GROUND:
        case ENKANBAN_WATER:
            signpost = (EnKanban*)thisv->actor.parent;

            if (signpost->partFlags == 0xFFFF) {
                Actor_Kill(&thisv->actor);
            }
            Math_ApproachF(&thisv->actor.shape.yOffset, 100.0f, 1.0f, 5.0f);
            if (thisv->actionState == ENKANBAN_WATER) {
                s32 rippleDelay;
                s32 rippleScale;

                if ((player->actor.speedXZ > 0.0f) && (player->actor.world.pos.y < thisv->actor.world.pos.y) &&
                    (thisv->actor.xyzDistToPlayerSq < 2500.0f)) {
                    Math_ApproachF(&thisv->actor.speedXZ, player->actor.speedXZ, 1.0f, 0.2f);
                    if (thisv->actor.speedXZ > 1.0f) {
                        thisv->actor.speedXZ = 1.0f;
                    }
                    if (Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer + 0x8000, 1, 0x1000,
                                           0) > 0) {
                        thisv->spinVel.y = thisv->actor.speedXZ * 1000.0f;
                    } else {
                        thisv->spinVel.y = thisv->actor.speedXZ * -1000.0f;
                    }
                }
                if (thisv->actor.bgCheckFlags & 1) {
                    thisv->actor.speedXZ = 0.0f;
                }
                Actor_MoveForward(&thisv->actor);
                if (thisv->actor.speedXZ != 0.0f) {
                    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 50.0f, 5);
                    if (thisv->actor.bgCheckFlags & 8) {
                        thisv->actor.speedXZ *= -0.5f;
                        if (thisv->spinVel.y > 0) {
                            thisv->spinVel.y = -0x7D0;
                        } else {
                            thisv->spinVel.y = 0x7D0;
                        }
                    }
                    Math_ApproachZeroF(&thisv->actor.speedXZ, 1.0f, 0.15f);
                }
                thisv->actor.shape.rot.y += thisv->spinVel.y;
                Math_ApproachS(&thisv->spinVel.y, 0, 1, 0x3A);
                Math_ApproachS(&thisv->actor.shape.rot.x, thisv->direction * 0x4000, 2, 0x1000);
                Math_ApproachS(&thisv->spinRot.x, Math_SinS(2500 * thisv->frameCount) * 500.0f, 2, 0x1000);
                Math_ApproachS(&thisv->spinRot.z, Math_CosS(3000 * thisv->frameCount) * 500.0f, 2, 0x1000);
                Math_ApproachZeroF(&thisv->floorRot.x, 0.5f, 0.2f);
                Math_ApproachZeroF(&thisv->floorRot.z, 0.5f, 0.2f);
                if (fabsf(thisv->actor.speedXZ) > 1.0f) {
                    rippleDelay = 0;
                } else if (fabsf(thisv->actor.speedXZ) > 0.5f) {
                    rippleDelay = 3;
                } else {
                    rippleDelay = 7;
                }
                if (!(thisv->frameCount & rippleDelay)) {
                    if (thisv->partCount < 3) {
                        rippleScale = 0;
                    } else if (thisv->partCount < 6) {
                        rippleScale = 100;
                    } else {
                        rippleScale = 200;
                    }
                    EffectSsGRipple_Spawn(globalCtx, &thisv->actor.world.pos, rippleScale, rippleScale + 500, 0);
                }
            } else if ((globalCtx->actorCtx.unk_02 != 0) && (thisv->actor.xyzDistToPlayerSq < SQ(100.0f))) {
                f32 hammerStrength = (100.0f - sqrtf(thisv->actor.xyzDistToPlayerSq)) * 0.05f;

                thisv->actionState = ENKANBAN_AIR;
                thisv->actor.gravity = -1.0f;
                thisv->actor.world.rot.y = Rand_CenteredFloat(0x10000);
                if (thisv->partCount >= 4) {
                    thisv->bounceX = (s16)Rand_ZeroFloat(10.0f) + 6;
                    thisv->bounceZ = (s16)Rand_ZeroFloat(10.0f) + 6;
                    thisv->actor.velocity.y = 2.0f + hammerStrength;
                    thisv->actor.speedXZ = Rand_ZeroFloat(1.0f);
                } else {
                    thisv->bounceX = (s16)Rand_ZeroFloat(7.0f) + 3;
                    thisv->bounceZ = (s16)Rand_ZeroFloat(7.0f) + 3;
                    thisv->actor.velocity.y = 3.0f + hammerStrength;
                    thisv->actor.speedXZ = Rand_ZeroFloat(1.5f);
                }
                thisv->spinVel.y = Rand_CenteredFloat(0x1800);
                if (Rand_ZeroOne() < 0.5f) {
                    thisv->direction = 1;
                } else {
                    thisv->direction = -1;
                }
                thisv->airTimer = 70;
            }
            if (thisv->bounceX == 0) {
                Actor* bomb = globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
                f32 dx;
                f32 dy;
                f32 dz;

                while (bomb != NULL) {
                    if (bomb->params != 1) {
                        bomb = bomb->next;
                        continue;
                    }
                    dx = thisv->actor.world.pos.x - bomb->world.pos.x;
                    dy = thisv->actor.world.pos.y - bomb->world.pos.y;
                    dz = thisv->actor.world.pos.z - bomb->world.pos.z;
                    if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f) {
                        f32 bombStrength = (100.0f - sqrtf(SQ(dx) + SQ(dy) + SQ(dz))) * 0.05f;

                        thisv->actionState = ENKANBAN_AIR;
                        thisv->actor.gravity = -1.0f;
                        thisv->actor.world.rot.y = Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>);
                        if (thisv->partCount >= 4) {
                            thisv->bounceX = (s16)Rand_ZeroFloat(10.0f) + 6;
                            thisv->bounceZ = (s16)Rand_ZeroFloat(10.0f) + 6;
                            thisv->actor.velocity.y = 2.5f + bombStrength;
                            thisv->actor.speedXZ = 3.0f + bombStrength;
                        } else {
                            thisv->bounceX = (s16)Rand_ZeroFloat(7.0f) + 3;
                            thisv->bounceZ = (s16)Rand_ZeroFloat(7.0f) + 3;
                            thisv->actor.velocity.y = 5.0f + bombStrength;
                            thisv->actor.speedXZ = 4.0f + bombStrength;
                        }
                        thisv->spinVel.y = Rand_CenteredFloat(0x1800);
                        if (Rand_ZeroOne() < 0.5f) {
                            thisv->direction = 1;
                        } else {
                            thisv->direction = -1;
                        }
                        thisv->airTimer = 70;
                    }
                    bomb = bomb->next;
                }
            }
            osSyncPrintf(VT_FGCOL(GREEN));
            osSyncPrintf("OCARINA_MODE %d\n", globalCtx->msgCtx.ocarinaMode);
            osSyncPrintf(VT_RST);
            switch (thisv->ocarinaFlag) {
                case 0:
                    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
                        thisv->ocarinaFlag = 1;
                    }
                    break;
                case 1:
                    if ((globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) &&
                        (globalCtx->msgCtx.unk_E3F2 == OCARINA_SONG_LULLABY)) {
                        thisv->actionState = ENKANBAN_REPAIR;
                        thisv->bounceX = 1;
                        Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    }
                    break;
            }
            break;
        case ENKANBAN_REPAIR: {
            f32 distX;
            f32 distY;
            f32 distZ;
            s16 pDiff;
            s16 yDiff;
            s16 rDiff;
            signpost = (EnKanban*)thisv->actor.parent;

            if (signpost->partFlags == 0xFFFF) {
                Actor_Kill(&thisv->actor);
            }

            Matrix_RotateY((signpost->actor.shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_NEW);
            Matrix_MultVec3f(&sPieceOffsets[thisv->pieceType], &offset);
            distX =
                Math_SmoothStepToF(&thisv->actor.world.pos.x, signpost->actor.world.pos.x + offset.x, 1.0f, 3.0f, 0.0f);
            distY =
                Math_SmoothStepToF(&thisv->actor.world.pos.y, signpost->actor.world.pos.y + offset.y, 1.0f, 3.0f, 0.0f);
            distZ =
                Math_SmoothStepToF(&thisv->actor.world.pos.z, signpost->actor.world.pos.z + offset.z, 1.0f, 3.0f, 0.0f);
            pDiff = Math_SmoothStepToS(&thisv->actor.shape.rot.x, signpost->actor.shape.rot.x, 1, 0x200, 0);
            yDiff = Math_SmoothStepToS(&thisv->actor.shape.rot.y, signpost->actor.shape.rot.y, 1, 0x200, 0);
            rDiff = Math_SmoothStepToS(&thisv->actor.shape.rot.z, signpost->actor.shape.rot.z, 1, 0x200, 0);
            Math_ApproachS(&thisv->spinRot.x, 0, 1, 0x200);
            Math_ApproachS(&thisv->spinRot.z, 0, 1, 0x200);
            Math_ApproachZeroF(&thisv->floorRot.x, 1.0f, 0.05f);
            Math_ApproachZeroF(&thisv->floorRot.z, 1.0f, 0.05f);
            Math_ApproachZeroF(&thisv->actor.shape.yOffset, 1.0f, 2.0f);
            if (((distX + distY + distZ) == 0.0f) &&
                ((pDiff + yDiff + rDiff + thisv->spinRot.x + thisv->spinRot.z) == 0) && (thisv->floorRot.x == 0.0f) &&
                (thisv->floorRot.z == 0.0f)) {
                signpost->partFlags |= thisv->partFlags;
                signpost->actor.flags |= ACTOR_FLAG_0;
                Actor_Kill(&thisv->actor);
            }
        } break;
    }
}

static Gfx* sDisplayLists[] = {
    object_kanban_DL_000CB0, object_kanban_DL_000DB8, object_kanban_DL_000E78, object_kanban_DL_000F38,
    object_kanban_DL_000FF8, object_kanban_DL_0010B8, object_kanban_DL_0011C0, object_kanban_DL_0012C8,
    object_kanban_DL_0013D0, object_kanban_DL_001488, object_kanban_DL_001540,
};

#include "z_en_kanban_gfx.c"

static f32 sCutAngles[] = {
    /* CUT_POST   */ 0.50f * std::numbers::pi_v<float>,
    /* CUT_VERT_L */ 0.00f * std::numbers::pi_v<float>,
    /* CUT_HORIZ  */ 0.50f * std::numbers::pi_v<float>,
    /* CUT_DIAG_L */ 0.66f * std::numbers::pi_v<float>,
    /* CUT_DIAG_R */ 0.34f * std::numbers::pi_v<float>,
    /* CUT_VERT_R */ 0.00f * std::numbers::pi_v<float>,
};

static s32 sUnused[] = { 0, 0, 0 }; // Unused zero vector?

#include "overlays/ovl_En_Kanban/ovl_En_Kanban.h"

void EnKanban_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnKanban* thisv = (EnKanban*)thisx;
    f32 zShift;
    f32 zShift2;
    s16 i;
    u8* shadowTex = Graph_Alloc(globalCtx->state.gfxCtx, 0x400);

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_kanban.c", 1659);
    func_80093D18(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_kanban_DL_000C30);
    if (thisv->actionState != ENKANBAN_SIGN) {
        Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateX(thisv->floorRot.x, MTXMODE_APPLY);
        Matrix_RotateZ(thisv->floorRot.z, MTXMODE_APPLY);
        Matrix_Translate(0.0f, thisv->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
        Matrix_RotateY((thisv->actor.shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateX((thisv->actor.shape.rot.x / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        zShift = fabsf(Math_SinS(thisv->spinRot.x) * thisv->pieceHeight);
        zShift2 = fabsf(Math_SinS(thisv->spinRot.z) * thisv->pieceWidth);
        zShift = MAX(zShift2, zShift);
        zShift *= -(f32)thisv->direction;
        Matrix_Translate(0.0f, 0.0f, zShift, MTXMODE_APPLY);
        Matrix_RotateX((thisv->spinRot.x / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateY((thisv->spinRot.z / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_Translate(thisv->offset.x, thisv->offset.y, thisv->offset.z - 100.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_kanban.c", 1715),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        for (i = 0; i < ARRAY_COUNT(sPartFlags); i++) {
            if (sPartFlags[i] & thisv->partFlags) {
                gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[i]);
            }
        }
    } else {
        Matrix_Translate(0.0f, 0.0f, -100.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_kanban.c", 1725),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (thisv->partFlags == 0xFFFF) {
            gSPDisplayList(POLY_OPA_DISP++, gSignRectangularDL);
        } else {
            for (i = 0; i < ARRAY_COUNT(sPartFlags); i++) {
                if (sPartFlags[i] & thisv->partFlags) {
                    gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[i]);
                }
            }
        }
        if (thisv->cutMarkAlpha != 0) {
            f32 cutOffset = (thisv->cutType == CUT_POST) ? -1200.0f : 0.0f;

            Matrix_Translate(0.0f, 4400.0f + cutOffset, 200.0f, MTXMODE_APPLY);
            Matrix_RotateZ(sCutAngles[thisv->cutType], MTXMODE_APPLY);
            Matrix_Scale(0.0f, 10.0f, 2.0f, MTXMODE_APPLY);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 255, 255, 255, thisv->cutMarkAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 150, 0);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_kanban.c", 1773),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_kanban_DL_001630);
        }
    }
    if ((thisv->actor.projectedPos.z <= 400.0f) && (thisv->actor.projectedPos.z > 0.0f) &&
        (thisv->actor.floorHeight > -3000.0f)) {
        if ((thisv->bounceX != 0) || (thisv->bounceZ != 0)) {
            u16 dayTime = gSaveContext.dayTime;
            f32 shadowAlpha;

            if (dayTime >= 0x8000) {
                dayTime = 0xFFFF - dayTime;
            }
            shadowAlpha = (dayTime * 0.00275f) + 10.0f;
            if (thisv->actor.projectedPos.z > 300.0f) {
                shadowAlpha *= ((400.0f - thisv->actor.projectedPos.z) * 0.01f);
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 0, (s8)shadowAlpha);

            if ((thisv->actionState == ENKANBAN_SIGN) && LINK_IS_CHILD) {
                zShift = 0.0f;
            } else {
                zShift = ((thisv->actor.world.pos.y - thisv->actor.floorHeight) * -50.0f) / 100.0f;
            }

            Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.floorHeight, thisv->actor.world.pos.z + zShift,
                             MTXMODE_NEW);
            Matrix_RotateX(thisv->floorRot.x, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->floorRot.z, MTXMODE_APPLY);
            Matrix_Scale(thisv->actor.scale.x, 0.0f, thisv->actor.scale.z, MTXMODE_APPLY);
            if (thisv->actionState == ENKANBAN_SIGN) {
                Matrix_RotateX(-std::numbers::pi_v<float> / 5, MTXMODE_APPLY);
            }
            Matrix_RotateY((thisv->actor.shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateX((thisv->actor.shape.rot.x / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateX((thisv->spinRot.x / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateY((thisv->spinRot.z / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_Translate(thisv->offset.x, thisv->offset.y, thisv->offset.z, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_kanban.c", 1833),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            for (i = 0; i < 0x400; i++) {
                if (sShadowTexFlags[i] & thisv->partFlags) {
                    shadowTex[i] = 0xFF;
                } else {
                    shadowTex[i] = 0;
                }
            }
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(shadowTex));
            gSPDisplayList(POLY_XLU_DISP++, sShadowDL);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_kanban.c", 1857);
}
