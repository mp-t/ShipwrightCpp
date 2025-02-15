/*
 * File: z_en_zl3.c
 * Overlay: ovl_En_Zl3
 * Description: Adult Zelda
 */

#include "z_en_zl3.h"

#include "vt.h"
#include "overlays/actors/ovl_En_Encount2/z_en_encount2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "objects/object_zl2/object_zl2.h"
#include "objects/object_zl2_anime2/object_zl2_anime2.h"

#define FLAGS ACTOR_FLAG_4

void EnZl3_Init(Actor* thisx, GlobalContext* globalCtx);
void EnZl3_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnZl3_Update(Actor* thisx, GlobalContext* globalCtx);
void EnZl3_Draw(Actor* thisx, GlobalContext* globalCtx);
void func_80B59AD0(EnZl3* thisv, GlobalContext* globalCtx);
void EnZl3_Reset(void);

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
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
    { 25, 80, 0, { 0, 0, 0 } },
};

static const void* sEyeTextures[] = { gZelda2EyeOpenTex, gZelda2EyeHalfTex, gZelda2EyeShutTex, gZelda2Eye03Tex,
                                gZelda2Eye04Tex,   gZelda2Eye05Tex,   gZelda2Eye06Tex,   NULL };

static const void* sMouthTextures[] = { gZelda2MouthSeriousTex, gZelda2MouthHappyTex, gZelda2MouthOpenTex };

static s32 D_80B5A468 = 0;

static Vec3f D_80B5A46C = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B5A478 = { 0.0f, 10.0f, 0.0f };

static f32 D_80B5A484 = 0.0f;

static Vec3f D_80B5A488 = { 0.0f, 0.0f, 0.0f };

static s32 D_80B5A494 = -1;

static Vec3f D_80B5A498 = { 148.0f, 260.0f, -87.0f };

static Vec3f D_80B5A4A4 = { -12.0f, 260.0f, -147.0f };

static Vec3f D_80B5A4B0 = { 42.0f, 260.0f, 13.0f };

static u32 D_80B5A4BC = 0;

void func_80B533B0(Actor* thisx, GlobalContext* globalCtx) {
    EnZl3* thisv = (EnZl3*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
}

void func_80B533FC(EnZl3* thisv, GlobalContext* globalCtx) {
    ColliderCylinder* collider = &thisv->collider;
    s32 pad[4];

    Collider_UpdateCylinder(&thisv->actor, collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &collider->base);
}

void EnZl3_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnZl3* thisv = (EnZl3*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80B53468(void) {
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_ESCAPE);
}

BossGanon2* func_80B53488(EnZl3* thisv, GlobalContext* globalCtx) {
    if (thisv->ganon == NULL) {
        Actor* actorIt = globalCtx->actorCtx.actorLists[ACTORCAT_BOSS].head;

        while (actorIt != NULL) {
            if (actorIt->id == ACTOR_BOSS_GANON2) {
                thisv->ganon = (BossGanon2*)actorIt;
                break;
            }
            actorIt = actorIt->next;
        }
    }
    return thisv->ganon;
}

void EnZl3_UpdateEyes(EnZl3* thisv) {
    s32 pad[2];
    s16* eyeTexIndex = &thisv->eyeTexIndex;
    s16* blinkTimer = &thisv->blinkTimer;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }
    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
}

void EnZl3_setEyeIndex(EnZl3* thisv, s16 index) {
    thisv->eyeTexIndex = index;
}

void EnZl3_setMouthIndex(EnZl3* thisv, s16 index) {
    thisv->mouthTexIndex = index;
}

void func_80B5357C(EnZl3* thisv, GlobalContext* globalCtx) {
    Vec3f* thisPos = &thisv->actor.world.pos;
    Vec3f sp20;

    sp20.x = thisPos->x + ((Rand_ZeroOne() - 0.5f) * 10.0f);
    sp20.y = thisPos->y;
    sp20.z = thisPos->z + ((Rand_ZeroOne() - 0.5f) * 10.0f);
    Item_DropCollectible(globalCtx, &sp20, 3);
}

void func_80B53614(EnZl3* thisv, GlobalContext* globalCtx) {
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_RIVER_SOUND, -442.0f, 4102.0f, -371.0f, 0, 0, 0, 0x12);
}

void func_80B5366C(EnZl3* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 5);
}

void func_80B536B4(EnZl3* thisv) {
    thisv->actor.bgCheckFlags &= ~0x9;
}

void func_80B536C4(EnZl3* thisv) {
    s32 pad[2];
    Vec3s* vec1 = &thisv->unk_3F8.unk_08;
    Vec3s* vec2 = &thisv->unk_3F8.unk_0E;

    Math_SmoothStepToS(&vec1->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&vec1->y, 0, 20, 6200, 100);
    Math_SmoothStepToS(&vec2->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&vec2->y, 0, 20, 6200, 100);
}

void func_80B53764(EnZl3* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_3F8.unk_18 = player->actor.world.pos;
    thisv->unk_3F8.unk_14 = kREG(16) - 16.0f;
    func_80034A14(&thisv->actor, &thisv->unk_3F8, kREG(17) + 0xC, 2);
}

s32 func_80B537E8(EnZl3* thisv) {
    s16 yawTowardsPlayer = thisv->actor.yawTowardsPlayer;
    s16* rotY = &thisv->actor.world.rot.y;
    s16* unk_3D0 = &thisv->unk_3D0;
    s16 retVal;
    s16 pad[2];

    Math_SmoothStepToS(unk_3D0, ABS((s16)(yawTowardsPlayer - *rotY)), 5, 6200, 100);
    retVal = Math_SmoothStepToS(rotY, yawTowardsPlayer, 5, *unk_3D0, 100);
    thisv->actor.shape.rot.y = *rotY;
    return retVal;
}

void func_80B538B0(EnZl3* thisv) {
    s16 yawTowardsPlayer = thisv->actor.yawTowardsPlayer;
    s16* rotY = &thisv->actor.world.rot.y;

    if (ABS((s16)(yawTowardsPlayer - *rotY)) >= 0x1556) {
        D_80B5A468 = 1;
    }

    if (D_80B5A468 != 0) {
        if (!func_80B537E8(thisv)) {
            D_80B5A468 = 0;
        }
    } else {
        thisv->unk_3D0 = 0;
    }
}

s32 EnZl3_UpdateSkelAnime(EnZl3* thisv) {
    return SkelAnime_Update(&thisv->skelAnime);
}

s32 func_80B5396C(EnZl3* thisv) {
    return thisv->unk_3C8;
}

void func_80B53974(EnZl3* thisv, u8 arg1) {
    thisv->unk_3C8 = arg1;
}

void func_80B53980(EnZl3* thisx, s16 y, s32 idx) {
    EnZl3* thisv = (EnZl3*)thisx; // thisv function might take thisx
    s32 action = thisv->action;
    s16 y2 = y;
    s32 yTemp;
    f32 curFrame;
    f32 unk_3DC;

    if (thisv->unk_2FC != 0) {
        SkelAnime* skelAnime = &thisv->skelAnime;
        s32 temp25C = thisv->unk_25C[idx];
        s32 temp28C = (s16)(y + thisv->unk_28C[idx]);
        s32 temp2BC = y - thisv->unk_2BC[idx];

        if ((s32)fabsf(temp2BC) > 0x8000) {
            if (y2 > 0) {
                temp2BC -= 0x10000;
            } else {
                temp2BC += 0x10000;
            }
        }

        if ((idx != 0 || action != 4) && (temp2BC != 0)) {
            temp25C += (temp2BC - temp25C) / 16;
        }

        if (temp25C != 0) {
            temp25C -= temp25C / 10;
        }

        if ((s16)(temp28C - y) != 0) {
            temp25C -= (s16)(temp28C - y) / 50;
        }

        temp28C += temp25C;
        if (((thisv->unk_25C[idx] * temp25C) <= 0) && (((s16)(temp28C - y) > -0x64) && ((s16)(temp28C - y) < 0x64))) {
            temp28C = y;
            temp25C = 0;
        }
        if (idx == 0 && action == 3) {
            yTemp = y + -11000;
            if (skelAnime->mode == 2) {
                curFrame = skelAnime->curFrame;
                unk_3DC = thisv->unk_3DC;
                yTemp = (s32)((curFrame / unk_3DC) * -11000) + y;
                if (0) {};
                if (temp28C >= yTemp) {
                    temp28C = yTemp;
                    if (temp25C > 0) {
                        temp25C /= -2;
                    }
                }
            } else {
                if (temp28C >= yTemp) {
                    temp28C = yTemp;
                    if (temp25C > 0) {
                        temp25C /= -2;
                    }
                }
            }
        }
        thisv->unk_25C[idx] = temp25C;
        thisv->unk_28C[idx] = temp28C - y;
    }
    thisv->unk_2BC[idx] = y;
}

void func_80B53B64(EnZl3* thisv, s16 z, s32 idx) {
    SkelAnime* skelAnime = &thisv->skelAnime;
    s32 action = thisv->action;
    s32 phi_a1;
    s32 idx25C;
    s16 temp_t1;
    s32 temp_a0;
    s32 phi_v0;
    s32 phi_v1;
    s32 phi_v1_2;

    if (idx == 2) {
        phi_a1 = 15000;
        idx25C = 0;
    } else if (idx == 5) {
        phi_a1 = 13000;
        idx25C = 3;
    } else if (idx == 8) {
        phi_a1 = 12000;
        idx25C = 6;
    } else if (idx == 11) {
        phi_a1 = 0x4000;
        idx25C = 9;
    } else if (idx == 14) {
        phi_a1 = 0x4000;
        idx25C = 12;
    } else if (idx == 17) {
        phi_a1 = 0x4000;
        idx25C = 15;
    } else {
        phi_a1 = 0x4000;
        idx25C = 18;
    }

    if (thisv->unk_2FC != 0) {
        phi_v0 = thisv->unk_25C[idx];
        temp_a0 = (s16)(z + thisv->unk_28C[idx]);
        phi_v1 = z - thisv->unk_2BC[idx];

        if ((s32)fabsf(phi_v1) > 0x8000) {
            if (z > 0) {
                phi_v1 -= 0x10000;
            } else {
                phi_v1 += 0x10000;
            }
        }

        if (idx25C >= 0) {
            phi_v1 += ABS(thisv->unk_25C[idx25C]) / 3;
        }

        if (idx == 2 && (action == 5 || action == 24)) {
            if (phi_v1 != 0) {
                phi_v0 -= (phi_v1 - phi_v0) / 10;
            }
        } else if (idx == 2 && action == 22 && skelAnime->mode == 2) {
            if (phi_v1 != 0) {
                phi_v0 -= (phi_v1 - phi_v0) / 10;
            }
        } else if (idx == 2 && (action == 20 || action == 21) && skelAnime->mode == 2) {
            if (phi_v1 != 0) {
                phi_v0 -= (phi_v1 - phi_v0) / 10;
            }
        } else {
            if (phi_v1 != 0) {
                phi_v0 += (phi_v1 - phi_v0) / 16;
            }
        }

        if (phi_v0 != 0) {
            phi_v0 -= phi_v0 / 10;
        }

        if ((s16)(temp_a0 - phi_a1) != 0) {
            phi_v0 -= (s16)(temp_a0 - phi_a1) / 50;
        }

        temp_a0 += phi_v0;
        phi_v1 = (s16)(temp_a0 - phi_a1);

        if (((thisv->unk_25C[idx] * phi_v0) <= 0) && (phi_v1 > -100) && (phi_v1 < 100)) {
            temp_a0 = phi_a1;
            phi_v0 = 0;
        }

        if (idx == 2) {
            if (action == 4) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E0 = thisv->unk_3E0;

                    phi_v1_2 = (s32)(((unk_3E0 - curFrame) / unk_3E0) * -2000.0f) + phi_a1;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 5) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E4 = thisv->unk_3E4;

                    phi_v1_2 = (s32)((curFrame / unk_3E4) * -2000.0f) + phi_a1;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    phi_v1_2 = phi_a1 - 2000;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if ((action == 20) || (action == 21)) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3F4 = thisv->unk_3F4;

                    if (curFrame <= 42.0f) {
                        phi_v1_2 = phi_a1 - 2000;
                    } else {
                        phi_v1_2 = (s32)((((curFrame - 42.0f) * 6200.0f) / (unk_3F4 - 42.0f)) + -2000.0f) + phi_a1;
                    }

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    phi_v1_2 = phi_a1 + 4200;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 22) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3EC = thisv->unk_3EC;

                    phi_v1_2 = (s32)(((curFrame / unk_3EC) * -5200.0f) + 4200.0f) + phi_a1;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    phi_v1_2 = phi_a1 - 2000;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 23) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3F0 = thisv->unk_3F0;

                    phi_v1_2 = (s32)(((curFrame / unk_3F0) * -7600.0f) + -2000.0f) + phi_a1;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    phi_v1_2 = phi_a1 - 9600;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 24) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E8 = thisv->unk_3E8;

                    phi_v1_2 = (s32)(((curFrame / unk_3E8) * 21000.0f) + -9600.0f) + phi_a1;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    phi_v1_2 = phi_a1 + 11400;
                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            }
        } else if (idx == 11 || idx == 17) {
            if (action == 4) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E0 = thisv->unk_3E0;

                    phi_v1_2 = (s32)((curFrame / unk_3E0) * -7000.0f) + phi_a1;
                    if (temp_a0 >= phi_v1_2) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 > 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    phi_v1_2 = phi_a1 - 7000;
                    if (temp_a0 >= phi_v1_2) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 > 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 5) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E4 = thisv->unk_3E4;

                    phi_v1_2 = (s32)(((unk_3E4 - curFrame) / unk_3E4) * -7000.0f) + phi_a1;
                    if (temp_a0 >= phi_v1_2) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 > 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            }
        }
        thisv->unk_25C[idx] = phi_v0;
        thisv->unk_28C[idx] = temp_a0 - z;
    }
    thisv->unk_2BC[idx] = z;
}

void func_80B54360(EnZl3* thisv, s16 arg1, s32 arg2) {
    if (thisv->unk_2FC != 0) {
        s32 temp_v1 = thisv->unk_28C[arg2] - arg1;
        s16 temp_t0 = temp_v1;
        s32 temp_t2 = temp_t0;
        s32 temp_t3 = thisv->unk_25C[arg2];
        s32 phi_v0 = temp_t3;
        s32 index25C;
        s32 phi_t5;

        temp_t3 = arg1 - thisv->unk_2BC[arg2];

        if (arg2 == 1) {
            index25C = 0;
            phi_t5 = thisv->unk_25C[index25C];
        } else if (arg2 == 4) {
            index25C = 3;
            phi_t5 = thisv->unk_25C[index25C];
        } else if (arg2 == 7) {
            index25C = 6;
            phi_t5 = thisv->unk_25C[index25C];
        } else if (arg2 == 10) {
            index25C = 9;
            phi_t5 = ABS(thisv->unk_25C[index25C]);
        } else if (arg2 == 13) {
            index25C = 12;
            phi_t5 = ABS(thisv->unk_25C[index25C]);
        } else if (arg2 == 16) {
            index25C = 15;
            phi_t5 = -ABS(thisv->unk_25C[index25C]);
        } else {
            index25C = 18;
            phi_t5 = -ABS(thisv->unk_25C[index25C]);
        }

        if ((s32)fabsf(temp_t3) > 0x8000) {
            if (arg1 > 0) {
                temp_t3 -= 0x10000;
            } else {
                temp_t3 += 0x10000;
            }
        }
        if (index25C >= 0) {
            temp_t3 += phi_t5 / 3;
        }

        if (temp_t3 != 0) {
            phi_v0 += (temp_t3 - phi_v0) / 16;
        }
        if (phi_v0 != 0) {
            phi_v0 -= phi_v0 / 10;
        }
        if (temp_t0 != 0) {
            phi_v0 -= temp_t0 / 50;
        }
        temp_v1 += phi_v0;
        if (((thisv->unk_25C[arg2] * phi_v0) <= 0) && (temp_t2 > -0x64) && (temp_t2 < 0x64)) {
            temp_v1 = 0;
            phi_v0 = 0;
        }
        thisv->unk_25C[arg2] = phi_v0;
        thisv->unk_28C[arg2] = arg1 + temp_v1;
    }
    thisv->unk_2BC[arg2] = arg1;
}

s32 func_80B5458C(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                  Gfx** gfx) {
    s32 pad[3];
    EnZl3* thisv = (EnZl3*)thisx;
    s16* unk_28C = thisv->unk_28C;
    Mtx* sp78;
    MtxF sp38;
    Vec3s sp30;
    Vec3s* unk_3F8_unk_08 = &thisv->unk_3F8.unk_08;
    Vec3s* unk_3F8_unk_0E = &thisv->unk_3F8.unk_0E;

    if (limbIndex == 14) {
        sp78 = static_cast<Mtx*>(Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Mtx) * 7));
        rot->x += unk_3F8_unk_08->y;
        rot->z += unk_3F8_unk_08->x;
        gSPSegment((*gfx)++, 0x0C, sp78);

        Matrix_Push();
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Translate(362.0f, -133.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(globalCtx)) {
            func_80B53980(thisv, sp30.y, 0);
            func_80B54360(thisv, sp30.x, 1);
            func_80B53B64(thisv, sp30.z, 2);
        }
        Matrix_RotateZYX(unk_28C[0] + kREG(31), unk_28C[1] + kREG(32), unk_28C[2] + kREG(33), MTXMODE_APPLY);
        Matrix_Translate(-188.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sp78[0], "../z_en_zl3.c", 1490);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(globalCtx)) {
            func_80B53980(thisv, sp30.y, 3);
        }
        Matrix_RotateZYX(unk_28C[3] + kREG(34), unk_28C[4] + kREG(35), unk_28C[5] + kREG(36), MTXMODE_APPLY);
        Matrix_Translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sp78[1], "../z_en_zl3.c", 1534);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(globalCtx)) {
            func_80B54360(thisv, sp30.x, 7);
        }
        Matrix_RotateZYX(unk_28C[6] + kREG(37), unk_28C[7] + kREG(38), unk_28C[8] + kREG(39), MTXMODE_APPLY);
        Matrix_Translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sp78[2], "../z_en_zl3.c", 1554);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, 389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(globalCtx)) {
            func_80B53980(thisv, sp30.y, 9);
            func_80B54360(thisv, sp30.x, 10);
            func_80B53B64(thisv, sp30.z, 11);
        }
        Matrix_RotateZYX(unk_28C[9] + kREG(40), unk_28C[10] + kREG(41), unk_28C[11] + kREG(42), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, -3.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sp78[3], "../z_en_zl3.c", 1579);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(globalCtx)) {
            func_80B53980(thisv, sp30.y, 12);
            func_80B54360(thisv, sp30.x, 13);
            func_80B53B64(thisv, sp30.z, 14);
        }
        Matrix_RotateZYX(unk_28C[12] + kREG(43), unk_28C[13] + kREG(44), unk_28C[14] + kREG(45), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sp78[4], "../z_en_zl3.c", 1598);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, -389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(globalCtx)) {
            func_80B53980(thisv, sp30.y, 15);
            func_80B54360(thisv, sp30.x, 16);
            func_80B53B64(thisv, sp30.z, 17);
        }
        Matrix_RotateZYX(unk_28C[15] + kREG(46), unk_28C[16] + kREG(47), unk_28C[17] + kREG(48), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, 3.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sp78[5], "../z_en_zl3.c", 1623);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(globalCtx)) {
            func_80B53980(thisv, sp30.y, 18);
            func_80B54360(thisv, sp30.x, 19);
            func_80B53B64(thisv, sp30.z, 20);
        }
        Matrix_RotateZYX(unk_28C[18] + kREG(49), unk_28C[19] + kREG(50), unk_28C[20] + kREG(51), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sp78[6], "../z_en_zl3.c", 1642);
        Matrix_Pop();
        Matrix_Pop();
        thisv->unk_2FC = 1;
    } else if (limbIndex == 7) {
        rot->x += unk_3F8_unk_0E->y;
        rot->y -= unk_3F8_unk_0E->x;
    }
    return false;
}

void EnZl3_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZl3* thisv = (EnZl3*)thisx;
    s32 pad;
    Vec3f sp34;
    s32 pad2;
    Vec3f sp24;
    Vec3f sp18;

    if (limbIndex == 13) {
        sp34 = D_80B5A46C;
        Matrix_MultVec3f(&sp34, &thisv->unk_31C);
    } else if (limbIndex == 14) {
        sp24 = D_80B5A478;
        Matrix_MultVec3f(&sp24, &sp18);
        thisv->actor.focus.pos.x = sp18.x;
        thisv->actor.focus.pos.y = sp18.y;
        thisv->actor.focus.pos.z = sp18.z;
        thisv->actor.focus.rot.x = thisv->actor.world.rot.x;
        thisv->actor.focus.rot.y = thisv->actor.world.rot.y;
        thisv->actor.focus.rot.z = thisv->actor.world.rot.z;
    }
}

s32 func_80B54DB4(EnZl3* thisv) {
    s32 params = thisv->actor.params >> 8;

    return params & 0xFF;
}

s32 func_80B54DC4(EnZl3* thisv) {
    s32 params = thisv->actor.params >> 4;

    return params & 0xF;
}

s32 func_80B54DD4(EnZl3* thisv) {
    s32 params = thisv->actor.params;

    return params & 0xF;
}

void func_80B54DE0(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 idx = thisv->unk_318;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[idx].segment);
}

void func_80B54E14(EnZl3* thisv, const AnimationHeader* animation, u8 arg2, f32 transitionRate, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&thisv->skelAnime, animation, playbackSpeed, unk0, fc, arg2, transitionRate);
}

void func_80B54EA4(EnZl3* thisv, GlobalContext* globalCtx) {
    f32 posX = thisv->actor.world.pos.x;
    f32 posY = thisv->actor.world.pos.y;
    f32 posZ = thisv->actor.world.pos.z;

    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_EG, posX, posY, posZ, 0, 0, 0, 0);
}

void func_80B54EF4(EnZl3* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void func_80B54F18(EnZl3* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_2F8 == 0) {
        f32 posX = thisv->actor.world.pos.x;
        f32 posY = thisv->actor.world.pos.y + (kREG(5) + -26.0f);
        f32 posZ = thisv->actor.world.pos.z;

        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0x4000,
                           0, WARP_PURPLE_CRYSTAL);
        thisv->unk_2F8 = 1;
    }
}

void func_80B54FB4(EnZl3* thisv, GlobalContext* globalCtx) {
    osSyncPrintf("ゼルダ姫のEn_Zl3_Actor_inFinal_Init通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    func_80B54E14(thisv, &gZelda2Anime2Anim_008AD0, 0, 0.0f, 0);
    EnZl3_setEyeIndex(thisv, 4);
    EnZl3_setMouthIndex(thisv, 2);
    thisv->action = 1;
    thisv->drawConfig = 1;
    func_80B54F18(thisv, globalCtx);
    thisv->actor.shape.rot.z = 0;
    thisv->unk_3C4 = thisv->actor.world.rot.z;
    thisv->actor.world.rot.z = thisv->actor.shape.rot.z;
    osSyncPrintf("ゼルダ姫のEn_Zl3_Actor_inFinal_Initは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void func_80B55054(EnZl3* thisv) {
    if (thisv->unk_328 != 0) {
        Actor* child = thisv->actor.child;

        if (child != NULL) {
            f32* temp_v0 = &thisv->unk_2EC;

            if (*temp_v0 < 19.0f) {
                ((DoorWarp1*)child)->crystalAlpha = (20.0f - *temp_v0) * 12.75f;
                *temp_v0 += 1.0f;
            } else {
                Actor_Kill(child);
                thisv->actor.child = NULL;
            }
        }
    }
}

void func_80B550F0(EnZl3* thisv) {
    Actor* child = thisv->actor.child;

    if (child != NULL) {
        child->world.pos.x = thisv->actor.world.pos.x;
        child->world.pos.y = thisv->actor.world.pos.y + (kREG(5) + -26.0f);
        child->world.pos.z = thisv->actor.world.pos.z;
    }
}

void func_80B55144(EnZl3* thisv) {
    f32* fl = &D_80B5A484;

    if (1) {} // necessary to match

    if (*fl < 2.0f) {
        *fl += 1.0f;
        EnZl3_setEyeIndex(thisv, 2);
    } else if (*fl < 4.0f) {
        *fl += 1.0f;
        EnZl3_setEyeIndex(thisv, 1);
    } else {
        EnZl3_UpdateEyes(thisv);
    }
}

void func_80B551E0(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_008AD0, 0, 0.0f, 0);
    thisv->action = 1;
}

void func_80B55220(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_0091D8, 2, 0.0f, 0);
    thisv->action = 2;
    EnZl3_setMouthIndex(thisv, 0);
}

void func_80B55268(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_0091D8, 2, 0.0f, 0);
    thisv->action = 3;
}

void func_80B552A8(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_0099A0, 0, 0.0f, 0);
    }
}

void func_80B552DC(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_00A598, 2, -8.0f, 0);
    func_80B54EF4(thisv);
    EnZl3_setMouthIndex(thisv, 2);
    thisv->action = 4;
    func_80B53468();
}

void func_80B55334(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_00AACC, 0, 0.0f, 0);
    }
}

void func_80B55368(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_00A334, 2, -8.0f, 0);
    EnZl3_setMouthIndex(thisv, 0);
    thisv->action = 5;
}

void func_80B553B4(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009FBC, 0, 0.0f, 0);
    }
}

void func_80B553E8(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B59AD0(thisv, globalCtx);
}

void func_80B55408(EnZl3* thisv) {
    Actor* child = thisv->actor.child;

    if (child != NULL) {
        Actor_Kill(child);
    }
    Actor_Kill(&thisv->actor);
}

void func_80B55444(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 temp_v0 = func_80B5396C(thisv);

    if (temp_v0 >= 0) {
        s32 unk_2F0 = thisv->unk_2F0;

        if (temp_v0 != unk_2F0) {
            switch (temp_v0) {
                case 0:
                    func_80B551E0(thisv);
                    break;
                case 1:
                    EnZl3_setEyeIndex(thisv, 3);
                    func_80B54EF4(thisv);
                    break;
                case 3:
                    func_80B55220(thisv);
                    break;
                case 4:
                    func_80B55268(thisv);
                    break;
                case 5:
                    func_80B552DC(thisv);
                    break;
                case 6:
                    func_80B55368(thisv);
                    break;
                case 7:
                    func_80B553E8(thisv, globalCtx);
                    break;
                case 2:
                    func_80B55408(thisv);
                    break;
                case 8:
                    thisv->unk_328 = 1;
                default:
                    osSyncPrintf("En_Zl3_inFinal_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_2F0 = temp_v0;
        }
    }
}

void func_80B55550(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B550F0(thisv);
    func_80B55444(thisv, globalCtx);
}

void func_80B555A4(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_setEyeIndex(thisv, 2);
    func_80B550F0(thisv);
    func_80B55054(thisv);
    func_80B55444(thisv, globalCtx);
}

void func_80B55604(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    func_80B55144(thisv);
    func_80B552A8(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B55054(thisv);
    func_80B55444(thisv, globalCtx);
}

void func_80B5566C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55334(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B55444(thisv, globalCtx);
}

void func_80B556CC(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B553B4(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B55444(thisv, globalCtx);
}

void func_80B5572C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B55444(thisv, globalCtx);
}

void func_80B55780(EnZl3* thisv, GlobalContext* globalCtx) {
    osSyncPrintf("ゼルダ姫のEn_Zl3_Actor_inFinal2_Init通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    func_80B54E14(thisv, &gZelda2Anime2Anim_005A0C, 0, 0.0f, 0);
    thisv->action = 7;
    thisv->drawConfig = 1;
    osSyncPrintf("ゼルダ姫のEn_Zl3_Actor_inFinal2_Initは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    EnZl3_setMouthIndex(thisv, 1);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void func_80B55808(EnZl3* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void func_80B5582C(EnZl3* thisv) {
    Audio_PlaySoundRandom(&D_80B5A488, NA_SE_VO_Z1_CRY_0, NA_SE_VO_Z1_CRY_1 - NA_SE_VO_Z1_CRY_0 + 1);
}

void func_80B5585C(EnZl3* thisv) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    if ((skelAnime->mode == 2) && Animation_OnFrame(skelAnime, 4.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_Z1_PAIN);
    }
}

void func_80B558A8(EnZl3* thisv) {
    s32 pad[4];
    s16 thisRotY = thisv->actor.world.rot.y;
    Vec3f* unk_338 = &thisv->unk_338;

    *unk_338 = thisv->unk_32C = thisv->actor.world.pos;

    unk_338->z += (-1.6074f * Math_CosS(thisRotY)) - (3.1620007f * Math_SinS(thisRotY));
    unk_338->x += (-1.6074f * Math_SinS(thisRotY)) + (3.1620007f * Math_CosS(thisRotY));
    unk_338->y += -0.012199402f;
}

void func_80B559C4(EnZl3* thisv) {
    Vec3f* thisPos = &thisv->actor.world.pos;
    Vec3f* unk_32C = &thisv->unk_32C;
    Vec3f* unk_338 = &thisv->unk_338;
    f32 temp_f0 = Environment_LerpWeightAccelDecel(Animation_GetLastFrame(&gZelda2Anime2Anim_005248), 0,
                                                   (s32)thisv->skelAnime.curFrame, 3, 3);

    thisPos->x = unk_32C->x + (temp_f0 * (unk_338->x - unk_32C->x));
    thisPos->z = unk_32C->z + (temp_f0 * (unk_338->z - unk_32C->z));
}

void func_80B55A58(EnZl3* thisv, GlobalContext* globalCtx) {
    if (globalCtx->activeCamera == MAIN_CAM) {
        func_80B537E8(thisv);
    }
}

void func_80B55A84(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_005A0C, 0, 0.0f, 0);
    thisv->action = 7;
}

void func_80B55AC4(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_00499C, 2, -8.0f, 0);
    thisv->action = 8;
}

void func_80B55B04(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_004408, 0, 0.0f, 0);
    }
}

void func_80B55B38(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_006508, 2, -8.0f, 0);
    thisv->action = 9;
}

void func_80B55B78(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_0061C4, 0, 0.0f, 0);
    }
}

void func_80B55BAC(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_005248, 2, -8.0f, 0);
    func_80B558A8(thisv);
    func_80B55808(thisv);
    EnZl3_setMouthIndex(thisv, 2);
    thisv->action = 10;
}

void func_80B55C0C(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_0054E0, 0, 0.0f, 0);
    thisv->action = 11;
}

void func_80B55C4C(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B55C0C(thisv);
    }
}

void func_80B55C70(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_008684, 2, -8.0f, 0);
    thisv->action = 12;
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void func_80B55CCC(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_006F04, 0, 0.0f, 0);
    }
}

void func_80B55D00(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->action = 13;
    } else if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x4300) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actor.textId = 0x70D5;
        func_8002F2F4(&thisv->actor, globalCtx);
    } else {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    }
}

void func_80B55DB0(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        thisv->action = 12;
    }
}

void func_80B55E08(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_006AB0, 2, -8.0f, 0);
    thisv->action = 14;
}

void func_80B55E48(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_008050, 0, 0.0f, 0);
    }
}

void func_80B55E7C(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_007A78, 2, -8.0f, 0);
    thisv->action = 15;
}

void func_80B55EBC(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_007C84, 0, 0.0f, 0);
    }
}

void func_80B55EF0(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_0082F8, 2, -8.0f, 0);
    thisv->action = 16;
    EnZl3_setMouthIndex(thisv, 0);
}

void func_80B55F38(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_003FF8, 0, 0.0f, 0);
    }
}

void func_80B55F6C(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->action = 0x12;
    } else if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x4300) {
        BossGanon2* bossGanon2 = func_80B53488(thisv, globalCtx);

        if ((bossGanon2 != NULL) && (bossGanon2->unk_324 <= (10.0f / 81.0f))) {
            thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
            thisv->actor.flags |= ACTOR_FLAG_0;
            thisv->actor.textId = 0x7059;
            func_8002F2F4(&thisv->actor, globalCtx);
        }
    } else {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    }
}

void func_80B5604C(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_007664, 2, -8.0f, 0);
    thisv->action = 17;
    func_80B5582C(thisv);
}

void func_80B56090(EnZl3* thisv, s32 arg1) {
    s32* unk_2F0 = &thisv->unk_2F0;

    if (func_80B5396C(thisv) == *unk_2F0) {
        if (arg1 != 0) {
            func_80B54E14(thisv, &gZelda2Anime2Anim_003FF8, 0, 0.0f, 0);
            thisv->action = 16;
            func_80B53974(thisv, 7);
            thisv->unk_2F0 = 7;
        }
    }
}

void func_80B56108(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        thisv->action = 16;
    }
}

void func_80B56160(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_0001D8, 0, 0.0f, 0);
    thisv->action = 19;
}

void func_80B561A0(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_001110, 2, -8.0f, 0);
    thisv->action = 20;
}

void func_80B561E0(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_0004F4, 0, 0.0f, 0);
    }
}

void func_80B56214(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->action = 21;
    } else if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x4300) {
        BossGanon2* bossGanon2 = func_80B53488(thisv, globalCtx);

        if (bossGanon2 != NULL) {
            if (bossGanon2->unk_324 <= (10.0f / 81.0f)) {
                thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
                thisv->actor.flags |= ACTOR_FLAG_0;
                thisv->actor.textId = 0x7059;
                func_8002F2F4(&thisv->actor, globalCtx);
            }
        }
    } else {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    }
}

void func_80B562F4(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        thisv->action = 20;
    }
}

void func_80B5634C(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_002348, 2, -8.0f, 0);
    thisv->action = 22;
}

void func_80B5638C(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_00210C, 0, 0.0f, 0);
    }
}

void func_80B563C0(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_002E54, 2, -8.0f, 0);
    thisv->action = 23;
}

void func_80B56400(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_002710, 0, 0.0f, 0);
    }
}

void func_80B56434(EnZl3* thisv) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_001D8C, 2, -8.0f, 0);
    thisv->action = 24;
}

void func_80B56474(EnZl3* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_0014DC, 0, 0.0f, 0);
    }
}

void func_80B564A8(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 temp_v0;
    s32* val = &D_80B5A494;

    temp_v0 = func_80B5396C(thisv);

    if (*val > 0) {
        *val -= 1;
    } else if (*val == 0) {
        *val -= 1;
        if (temp_v0 == 8) {
            func_80B5604C(thisv);
        }
    }

    if (temp_v0 >= 0) {
        s32 unk_2F0 = thisv->unk_2F0;

        if (temp_v0 != unk_2F0) {
            switch (temp_v0) {
                case 0:
                    func_80B55A84(thisv);
                    break;
                case 1:
                    func_80B55AC4(thisv);
                    break;
                case 2:
                    func_80B55B38(thisv);
                    break;
                case 3:
                    func_80B55BAC(thisv);
                    break;
                case 4:
                    func_80B55C70(thisv);
                    break;
                case 5:
                    func_80B55E08(thisv);
                    break;
                case 6:
                    func_80B55E7C(thisv);
                    break;
                case 7:
                    func_80B55EF0(thisv);
                    break;
                case 8:
                    *val = 10;
                    break;
                case 9:
                    func_80B56160(thisv);
                    break;
                case 10:
                    func_80B561A0(thisv);
                    break;
                case 11:
                    func_80B5634C(thisv);
                    break;
                case 12:
                    func_80B563C0(thisv);
                    break;
                case 13:
                    func_80B56434(thisv);
                    break;
                case 14:
                    Actor_Kill(&thisv->actor);
                    break;
                default:
                    osSyncPrintf("En_Zl3_inFinal2_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_2F0 = temp_v0;
        }
    }
}

void func_80B56658(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B564A8(thisv, globalCtx);
}

void func_80B566AC(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55B04(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
}

void func_80B5670C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55B78(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
}

void func_80B5676C(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 something;

    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    something = EnZl3_UpdateSkelAnime(thisv);
    func_80B559C4(thisv);
    func_80B55C4C(thisv, something);
}

void func_80B567CC(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B564A8(thisv, globalCtx);
}

void func_80B5682C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B55A58(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55CCC(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
    func_80B55D00(thisv, globalCtx);
}

void func_80B568B4(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B55DB0(thisv, globalCtx);
}

void func_80B5691C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55E48(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
}

void func_80B5697C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55EBC(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B5585C(thisv);
    func_80B564A8(thisv, globalCtx);
}

void func_80B569E4(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55F38(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
    func_80B55F6C(thisv, globalCtx);
}

void func_80B56A68(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 something;

    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    something = EnZl3_UpdateSkelAnime(thisv);
    func_80B564A8(thisv, globalCtx);
    func_80B56090(thisv, something);
}

void func_80B56AE0(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B55F38(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B56108(thisv, globalCtx);
}

void func_80B56B54(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B564A8(thisv, globalCtx);
}

void func_80B56BA8(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B561E0(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
    func_80B56214(thisv, globalCtx);
}

void func_80B56C24(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B562F4(thisv, globalCtx);
}

void func_80B56C84(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B5638C(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
}

void func_80B56CE4(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B56400(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
}

void func_80B56D44(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B56474(thisv, EnZl3_UpdateSkelAnime(thisv));
    func_80B564A8(thisv, globalCtx);
}

void func_80B56DA4(EnZl3* thisv) {
    func_800788CC(NA_SE_EV_ZELDA_POWER);
}

void func_80B56DC8(EnZl3* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void func_80B56DEC(EnZl3* thisv) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    if ((skelAnime->mode == 2) && Animation_OnFrame(skelAnime, 9.0f) != 0) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_Z1_OPENDOOR);
    }
}

void func_80B56E38(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s32 sfxId;
    SkelAnime* sp20 = &thisv->skelAnime;

    if ((Animation_OnFrame(sp20, 6.0f) || Animation_OnFrame(sp20, 0.0f)) && (thisv->actor.bgCheckFlags & 1)) {
        sfxId = 0x800;
        sfxId += SurfaceType_GetSfx(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
        func_80078914(&thisv->actor.projectedPos, sfxId);
    }
}

void func_80B56EB8(EnZl3* thisv, GlobalContext* globalCtx) {
    Flags_SetSwitch(globalCtx, func_80B54DB4(thisv));
}

s32 func_80B56EE4(EnZl3* thisv, GlobalContext* globalCtx) {
    return Flags_GetSwitch(globalCtx, func_80B54DB4(thisv));
}

void func_80B56F10(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 waypoint;
    Path* pathHead = globalCtx->setupPathList;

    if (pathHead != NULL) {
        waypoint = func_80B54DC4(thisv);
        pathHead += waypoint;
        thisv->unk_30C = pathHead;
        thisv->unk_310 = pathHead->count;
        osSyncPrintf("En_Zl3_Get_path_info レールデータをゲットだぜ = %d!!!!!!!!!!!!!!\n", waypoint);
    } else {
        osSyncPrintf("En_Zl3_Get_path_info レールデータが無い!!!!!!!!!!!!!!!!!!!!\n");
    }
}

s32 func_80B56F8C(EnZl3* thisv, s32 arg1) {
    s32 unk_310 = thisv->unk_310;

    if (unk_310 > arg1) {
        return 1;
    }
    return 0;
}

Vec3s* func_80B56FAC(EnZl3* thisv, s32 arg1) {
    Vec3s* point;
    Path* pathList = thisv->unk_30C;

    if ((pathList != NULL) && func_80B56F8C(thisv, arg1)) {
        point = &((Vec3s*)SEGMENTED_TO_VIRTUAL(pathList->points))[arg1];
        return point;
    }
    return NULL;
}

s32 func_80B57034(EnZl3* thisv, s32 arg1, s32 arg2) {
    Vec3s* vec1 = func_80B56FAC(thisv, arg1);
    Vec3s* vec2 = func_80B56FAC(thisv, arg2);

    if ((vec2 != NULL) && (vec1 != NULL)) {
        f32 xDiff = vec2->x - vec1->x;
        f32 zDiff = vec2->z - vec1->z;

        return ((xDiff == 0.0f) && (zDiff == 0.0f)) ? 0 : (s16)(Math_FAtan2F(xDiff, zDiff) * (0x8000 / std::numbers::pi_v<float>));
    }
    return 0;
}

s16 func_80B57104(EnZl3* thisv, s32 arg1) {
    Vec3s* point = func_80B56FAC(thisv, arg1);

    if (point != NULL) {
        f32 xDiff = point->x - thisv->actor.world.pos.x;
        f32 zDiff = point->z - thisv->actor.world.pos.z;

        if ((xDiff != 0.0f) || (zDiff != 0.0f)) {
            return Math_FAtan2F(xDiff, zDiff) * (0x8000 / std::numbers::pi_v<float>);
        }
    }
    return 0;
}

s32 func_80B571A8(EnZl3* thisv) {
    s32 pad;
    s32 unk_314 = thisv->unk_314;
    s32 pad2;

    if (func_80B56F8C(thisv, unk_314 + 1) == 0) {
        return thisv->actor.shape.rot.y;
    } else {
        return func_80B57034(thisv, unk_314, unk_314 + 1);
    }
}

s32 func_80B571FC(EnZl3* thisv) {
    s32 pad;
    s32 unk_314 = thisv->unk_314;

    if (func_80B56F8C(thisv, unk_314) == 0) {
        return thisv->actor.shape.rot.y;
    } else {
        return func_80B57104(thisv, unk_314);
    }
}

void func_80B57240(EnZl3* thisv) {
    s32 temp_a1 = func_80B571FC(thisv);
    s16* rotY = &thisv->actor.world.rot.y;

    Math_SmoothStepToS(rotY, temp_a1, 2, 6400, 1000);
    thisv->actor.shape.rot.y = *rotY;
}

void func_80B57298(EnZl3* thisv) {
    s16* rotY = &thisv->actor.world.rot.y;
    s16 temp_a1 = func_80B571A8(thisv);

    Math_SmoothStepToS(rotY, temp_a1, 2, 6400, 1000);
    thisv->actor.shape.rot.y = *rotY;
}

u16 func_80B572F0(GlobalContext* globalCtx) {
    s16 sceneNum = globalCtx->sceneNum;
    u16 ret;

    if (sceneNum == SCENE_GANON_SONOGO) {
        ret = 0x71A8;
    } else if (sceneNum == SCENE_GANON_FINAL) {
        ret = 0x71A9;
    } else {
        ret = 0x71AB;
    }
    return ret;
}

s32 func_80B57324(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        return 1;
    }
    return 0;
}

void func_80B57350(EnZl3* thisv, GlobalContext* globalCtx) {
    s16 temp_v0 = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (ABS(temp_v0) <= 0x4300) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        thisv->actor.textId = func_80B572F0(globalCtx);
        func_8002F2F4(&thisv->actor, globalCtx);
    }
}

s32 func_80B573C8(EnZl3* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        return 1;
    }
    return 0;
}

s32 func_80B573FC(EnZl3* thisv, GlobalContext* globalCtx, f32 arg2) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerX = player->actor.world.pos.x;
    f32 playerZ = player->actor.world.pos.z;
    f32 thisX = thisv->actor.world.pos.x;
    f32 thisZ = thisv->actor.world.pos.z;

    if (SQ(playerX - thisX) + SQ(playerZ - thisZ) < SQ(arg2)) {
        return 1;
    }
    return 0;
}

s32 func_80B57458(EnZl3* thisv, GlobalContext* globalCtx) {
    Vec3f* thisPos = &thisv->actor.world.pos;
    f32 thisX = thisPos->x;
    f32 thisZ = thisPos->z;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f* playerPos = &player->actor.world.pos;
    s32 pad;
    f32 playerX = playerPos->x;
    f32 playerZ = playerPos->z;
    f32 temp_f12 = playerX - thisX;
    f32 temp_f13 = playerZ - thisZ;
    s16 temp_v0;
    s16 temp_v1 = func_80B571A8(thisv);

    if (temp_f12 == 0.0f && temp_f13 == 0.0f) {
        return 1;
    }

    temp_v0 = (s16)(temp_v1 - (s16)(Math_FAtan2F(temp_f12, temp_f13) * (0x8000 / std::numbers::pi_v<float>)));

    if (temp_v0 < 0x1555) {
        return 1;
    } else if ((temp_v0 < 0x4000) && func_80B573FC(thisv, globalCtx, 150.0f)) {
        return 1;
    } else {
        return 0;
    }
}

s32 func_80B57564(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B573FC(thisv, globalCtx, 50.0f) || func_80B57458(thisv, globalCtx)) {
        return 1;
    }
    return 0;
}

s32 func_80B575B0(EnZl3* thisv, GlobalContext* globalCtx) {
    return func_80B573FC(thisv, globalCtx, 150.0f);
}

s32 func_80B575D0(EnZl3* thisv, GlobalContext* globalCtx) {
    return func_80B573FC(thisv, globalCtx, 50.0f);
}

s32 func_80B575F0(EnZl3* thisv, GlobalContext* globalCtx) {
    s16 sceneNum = globalCtx->sceneNum;

    if ((sceneNum == SCENE_GANON_SONOGO) && (func_80B54DB4(thisv) == 0x26)) {
        s32 unk_314 = thisv->unk_314;

        if (unk_314 == 1) {
            return 1;
        }
    }
    return 0;
}

void func_80B5764C(EnZl3* thisv, GlobalContext* globalCtx) {
    s16 sceneNum = globalCtx->sceneNum;

    if ((sceneNum == SCENE_GANON_SONOGO) && (func_80B54DB4(thisv) == 0x26)) {
        s32 unk_314 = thisv->unk_314 + 1;

        if ((unk_314 == 1) && !Gameplay_InCsMode(globalCtx)) {
            OnePointCutscene_Init(globalCtx, 1000, 40, &thisv->actor, MAIN_CAM);
        }
    }
}

s32 func_80B576C8(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B575F0(thisv, globalCtx) && (thisv->unk_3D8 == 0)) {
        return 1;
    }
    return 0;
}

void func_80B57704(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 unk_3C4 = thisv->unk_3C4;

    Flags_SetSwitch(globalCtx, unk_3C4);
}

void func_80B5772C(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 unk_3C4 = thisv->unk_3C4;

    Flags_UnsetSwitch(globalCtx, unk_3C4);
}

void func_80B57754(EnZl3* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.unk_13F0 == 0) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_OCEFF_WIPE4, thisv->actor.world.pos.x,
                    thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 1);
        func_80B56DA4(thisv);
    }
}

void func_80B577BC(GlobalContext* globalCtx, Vec3f* vec) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f* playerPos = &player->actor.world.pos;
    f32 posX = vec->x;
    f32 posY = vec->y;
    f32 posZ = vec->z;

    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_TEST, posX, posY, posZ, 0,
                (Math_FAtan2F(playerPos->x - posX, playerPos->z - posZ) * (0x8000 / std::numbers::pi_v<float>)), 0, 5);
}

void func_80B57858(GlobalContext* globalCtx) {
    func_80B577BC(globalCtx, &D_80B5A498);
    func_80B577BC(globalCtx, &D_80B5A4A4);
}

s32 func_80B57890(EnZl3* thisv, GlobalContext* globalCtx) {
    s8 pad[2];
    u8 curSpawn = globalCtx->curSpawn;
    s16 sceneNum = globalCtx->sceneNum;
    s32 result = func_80B54DB4(thisv);

    if (globalCtx) {} // Needed to match, thisv if can be almost anywhere and it still matches

    if (sceneNum == SCENE_GANON_SONOGO) {
        if ((result == 0x24) && (curSpawn == 0)) {
            return 1;
        }
        if ((result == 0x25) && (curSpawn == 2)) {
            return 1;
        }
        if ((result == 0x26) && (curSpawn == 4)) {
            return 1;
        }
        if ((result == 0x27) && (curSpawn == 6)) {
            return 1;
        }
        if ((result == 0x28) && (curSpawn == 6)) {
            return 1;
        }
    } else if (sceneNum == SCENE_GANON_FINAL) {
        if ((result == 0x20) && (curSpawn == 0) && Flags_GetSwitch(globalCtx, 0x37)) {
            if ((globalCtx->sceneNum == SCENE_GANON_DEMO) || (globalCtx->sceneNum == SCENE_GANON_FINAL) ||
                (globalCtx->sceneNum == SCENE_GANON_SONOGO) || (globalCtx->sceneNum == SCENE_GANONTIKA_SONOGO)) {
                return 1;
            }
        }
        if ((result == 0x21) && (curSpawn == 2)) {
            return 1;
        }
        if ((result == 0x22) && (curSpawn == 4)) {
            return 1;
        }
        if ((result == 0x23) && (curSpawn == 6)) {
            return 1;
        }
    } else if (sceneNum == SCENE_GANONTIKA_SONOGO) {
        if ((result == 0x29) && (curSpawn == 0)) {
            return 1;
        }
        if ((result == 0x2A) && (curSpawn == 0)) {
            return 1;
        }
    }
    return 0;
}

void func_80B57A74(GlobalContext* globalCtx) {
    Actor* actorIt = globalCtx->actorCtx.actorLists[ACTORCAT_PROP].head;
    //! @bug checks for encount2 in ACTORCAT_PROP but encount2 is in ACTORCAT_ENEMY so thisv condition is never met
    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_EN_ENCOUNT2) {
            ((EnEncount2*)actorIt)->collapseSpawnerInactive = true;
        }
        actorIt = actorIt->next;
    }
}

void func_80B57AAC(EnZl3* thisv, s32 arg1, const AnimationHeader* arg2) {
    if (arg1 != 0) {
        func_80B54E14(thisv, arg2, 0, -8.0f, 0);
    }
}

void func_80B57AE0(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 shapeRotY = thisv->actor.shape.rot.y;
    s32 pad2;
    Vec3f* unk_354 = &thisv->unk_354;
    Vec3f* unk_348 = &thisv->unk_348;
    Vec3s* temp_v0;
    f32 xDiff;
    f32 zDiff;

    thisv->unk_344 = 0;
    thisv->unk_314 += 1;
    thisv->unk_360 = 0.0f;
    thisv->unk_364 = 0.0f;
    thisv->unk_368 = 0.0f;
    *unk_348 = thisv->actor.world.pos;
    temp_v0 = func_80B56FAC(thisv, thisv->unk_314);

    if (temp_v0 != NULL) {
        unk_354->x = temp_v0->x;
        unk_354->y = temp_v0->y;
        unk_354->z = temp_v0->z;
    } else {
        unk_354->x = unk_348->x + (Math_SinS(shapeRotY) * 200.0f);
        unk_354->y = unk_348->y;
        unk_354->z = unk_348->z + (Math_CosS(shapeRotY) * 200.0f);
    }

    xDiff = unk_354->x - unk_348->x;
    zDiff = unk_354->z - unk_348->z;
    thisv->unk_346 = (s32)(sqrtf(SQ(xDiff) + SQ(zDiff)) / (kREG(6) + 8.0f));
}

s32 func_80B57C54(EnZl3* thisv) {
    if (thisv->unk_344 >= thisv->unk_346) {
        return 1;
    }
    return 0;
}

s32 func_80B57C7C(EnZl3* thisv, GlobalContext* globalCtx) {
    return 1;
}

s32 func_80B57C8C(EnZl3* thisv) {
    return !func_80B56F8C(thisv, thisv->unk_314 + 2);
}

void func_80B57CB4(EnZl3* thisv, GlobalContext* globalCtx) {
    Vec3f* unk_348 = &thisv->unk_348;
    Vec3f* unk_354 = &thisv->unk_354;
    Vec3f* thisPos = &thisv->actor.world.pos;
    f32 temp_f0;

    thisv->unk_344 += 1;
    temp_f0 = Environment_LerpWeightAccelDecel(thisv->unk_346, 0, thisv->unk_344, 3, 3);
    thisPos->x = unk_348->x + (temp_f0 * (unk_354->x - unk_348->x));
    thisPos->y = (unk_348->y + (temp_f0 * (unk_354->y - unk_348->y))) + thisv->unk_360;
    thisPos->z = unk_348->z + (temp_f0 * (unk_354->z - unk_348->z));
}

void func_80B57D60(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B57240(thisv);
}

s32 func_80B57D80(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16* sp32 = &thisv->actor.shape.rot.y;
    struct_80034A14_arg1* unk_3F8 = &thisv->unk_3F8;
    Player* player = GET_PLAYER(globalCtx);
    s32 unk_314 = thisv->unk_314;
    s16 temp_v0 = func_80B57104(thisv, unk_314);
    s32 pad2;
    s16 phi_v1;

    unk_3F8->unk_18.y = player->actor.world.pos.y;
    unk_3F8->unk_18.x = (Math_SinS(temp_v0) * thisv->actor.xzDistToPlayer) + thisv->actor.world.pos.x;
    unk_3F8->unk_18.z = (Math_CosS(temp_v0) * thisv->actor.xzDistToPlayer) + thisv->actor.world.pos.z;
    unk_3F8->unk_14 = kREG(16) - 16.0f;
    func_80034A14(&thisv->actor, unk_3F8, kREG(17) + 0xC, 4);

    phi_v1 = ABS(temp_v0 - *sp32);
    if (phi_v1 <= 0x320) {
        *sp32 = temp_v0;
        thisv->actor.world.rot.y = *sp32;
        phi_v1 = 0;
    }
    thisv->actor.world.rot.y = *sp32;
    return phi_v1;
}

void func_80B57EAC(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B57324(thisv, globalCtx)) {
        thisv->action = 26;
    } else {
        func_80B57350(thisv, globalCtx);
    }
}

void func_80B57EEC(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B573C8(thisv, globalCtx)) {
        thisv->action = 27;
    }
}

void func_80B57F1C(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B57D80(thisv, globalCtx) == 0) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
        thisv->action = 34;
        thisv->unk_314 -= 1;
        func_80B57AE0(thisv, globalCtx);
    }
}

s32 func_80B57F84(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B575D0(thisv, globalCtx) && func_80B57C7C(thisv, globalCtx) && !Gameplay_InCsMode(globalCtx)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009FBC, 0, -8.0f, 0);
        thisv->action = 36;
        thisv->unk_2EC = 0.0f;
        func_80B57A74(globalCtx);
        return 1;
    }
    return 0;
}

void func_80B58014(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s8 invincibilityTimer = player->invincibilityTimer;

    if (func_80B57324(thisv, globalCtx)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_003FF8, 0, -11.0f, 0);
        thisv->action = 29;
        func_80B538B0(thisv);
    } else if (func_80B57C8C(thisv) && func_80B57F84(thisv, globalCtx)) {
        OnePointCutscene_Init(globalCtx, 4000, -99, &thisv->actor, MAIN_CAM);
        thisv->unk_3D0 = 0;
    } else if (func_80B576C8(thisv, globalCtx) && func_80B575B0(thisv, globalCtx) && !Gameplay_InCsMode(globalCtx)) {
        thisv->action = 0x1F;
        thisv->unk_3CC = 0.0f;
        func_80B537E8(thisv);
        thisv->unk_3D8 = 1;
        OnePointCutscene_Init(globalCtx, 4010, -99, &thisv->actor, MAIN_CAM);
    } else if (!func_80B57C8C(thisv) && !func_80B576C8(thisv, globalCtx) && func_80B57564(thisv, globalCtx)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
        func_80B5764C(thisv, globalCtx);
        thisv->action = 34;
        thisv->unk_3D0 = 0;
        func_80B57AE0(thisv, globalCtx);
    } else if ((invincibilityTimer > 0) || (player->fallDistance >= 0x33)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_007664, 0, -11.0f, 0);
        thisv->action = 30;
        func_80B537E8(thisv);
        func_80B56DC8(thisv);
    } else {
        func_80B57350(thisv, globalCtx);
        func_80B538B0(thisv);
    }
}

void func_80B58214(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B573C8(thisv, globalCtx)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009FBC, 0, -11.0f, 0);
        thisv->action = 28;
        thisv->unk_3D0 = 0;
    }
}

void func_80B58268(EnZl3* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s8 invincibilityTimer = player->invincibilityTimer;

    if ((invincibilityTimer <= 0) && (player->fallDistance <= 50)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009FBC, 0, -11.0f, 0);
        thisv->action = 28;
        thisv->unk_3D0 = 0;
    }
}

void func_80B582C8(EnZl3* thisv, GlobalContext* globalCtx) {
    f32* unk_3CC = &thisv->unk_3CC;
    s32 pad;

    if (*unk_3CC == kREG(14) + 10.0f) {
        *unk_3CC += 1.0f;
        func_80B54E14(thisv, &gZelda2Anime2Anim_008050, 0, -12.0f, 0);
        func_80B57704(thisv, globalCtx);
    } else if (*unk_3CC == kREG(15) + 20.0f) {
        *unk_3CC += 1.0f;
        func_80B56DC8(thisv);
        func_80B54E14(thisv, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
    } else if (*unk_3CC == kREG(16) + 30.0f) {
        *unk_3CC += 1.0f;
        func_80B57858(globalCtx);
    } else if (*unk_3CC == kREG(17) + 40.0f) {
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
        *unk_3CC += 1.0f;
    } else if (*unk_3CC >= ((kREG(17) + 40.0f) + 1.0f)) {
        thisv->action = 32;
        *unk_3CC = 0.0f;
    } else {
        *unk_3CC += 1.0f;
    }
}

void func_80B584B4(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s8 invincibilityTimer = player->invincibilityTimer;
    Actor* nearbyEnTest = Actor_FindNearby(globalCtx, &thisv->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f);

    if (D_80B5A4BC == 0) {
        if ((nearbyEnTest == NULL) && (!Gameplay_InCsMode(globalCtx))) {
            thisv->action = 33;
            OnePointCutscene_Init(globalCtx, 4011, -99, &thisv->actor, MAIN_CAM);
        } else if (invincibilityTimer > 0) {
            func_80B54E14(thisv, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
            D_80B5A4BC = 1;
            func_80B56DC8(thisv);
        }
    } else {
        if ((nearbyEnTest == NULL) && (!Gameplay_InCsMode(globalCtx))) {
            func_80B54E14(thisv, &gZelda2Anime2Anim_007664, 0, -12.0f, 0);
            D_80B5A4BC = 0;
            thisv->action = 33;
            OnePointCutscene_Init(globalCtx, 4011, -99, &thisv->actor, MAIN_CAM);
        } else if (invincibilityTimer <= 0) {
            func_80B54E14(thisv, &gZelda2Anime2Anim_007664, 0, -12.0f, 0);
            D_80B5A4BC = 0;
        }
    }
}

void func_80B58624(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad[4];
    f32* unk_3CC = &thisv->unk_3CC;

    if (*unk_3CC == (kREG(18) + 10.0f)) {
        *unk_3CC += 1.0f;
        func_80B54E14(thisv, &gZelda2Anime2Anim_008050, 0, -12.0f, 0);
        func_80B5772C(thisv, globalCtx);
    } else if (*unk_3CC == kREG(19) + 20.0f) {
        *unk_3CC += 1.0f;
        thisv->actor.textId = 0x71AC;
        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        func_80B54E14(thisv, &gZelda2Anime2Anim_003FF8, 0, -12.0f, 0);
    } else if (*unk_3CC == ((kREG(19) + 20.0f) + 1.0f)) {
        if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
            *unk_3CC += 1.0f;
            func_80B5357C(thisv, globalCtx);
            func_80B5357C(thisv, globalCtx);
            func_80B5357C(thisv, globalCtx);
            func_80B5357C(thisv, globalCtx);
            func_80B5357C(thisv, globalCtx);
        }
    } else {
        if (*unk_3CC >= kREG(20) + 30.0f) {
            thisv->action = 28;
            func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
            func_80B54E14(thisv, &gZelda2Anime2Anim_009FBC, 0, -12.0f, 0);
            *unk_3CC = 0.0f;
        } else {
            *unk_3CC += 1.0f;
        }
    }
}

void func_80B5884C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_0038C0, 2, -8.0f, 0);
    thisv->action = 37;
    thisv->unk_36C = 1;
}

void func_80B58898(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_0038C0, 2, -8.0f, 1);
    thisv->action = 38;
    thisv->unk_374 = 1;
}

void func_80B588E8(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54E14(thisv, &gZelda2Anime2Anim_009BE4, 0, -8.0f, 0);
    func_80B57AE0(thisv, globalCtx);
    thisv->action = 39;
}

s32 func_80B58938(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B57C54(thisv)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009FBC, 0, -8.0f, 0);
        thisv->action = 28;
        thisv->unk_3D0 = 0;
        return 1;
    }
    return 0;
}

s32 func_80B5899C(EnZl3* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.bgCheckFlags & 1)) {
        Player* player = GET_PLAYER(globalCtx);
        s8 invincibilityTimer = player->invincibilityTimer;

        if ((invincibilityTimer > 0) || (player->fallDistance >= 0x33)) {
            func_80B54E14(thisv, &gZelda2Anime2Anim_007664, 2, -11.0f, 0);
            thisv->action = 35;
            func_80B56DC8(thisv);
            return 1;
        }
    }
    return 0;
}

void func_80B58A1C(EnZl3* thisv, GlobalContext* globalCtx) {
    if (!func_80B58938(thisv, globalCtx)) {
        func_80B5899C(thisv, globalCtx);
    }
}

void func_80B58A50(EnZl3* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s8 invincibilityTimer = player->invincibilityTimer;

    if ((invincibilityTimer <= 0) && (player->fallDistance <= 50)) {
        func_80B54E14(thisv, &gZelda2Anime2Anim_009BE4, 0, -11.0f, 0);
        thisv->action = 34;
    }
}

void func_80B58AAC(EnZl3* thisv, GlobalContext* globalCtx) {
    f32* unk_2EC = &thisv->unk_2EC;

    *unk_2EC += 1.0f;
    if ((*unk_2EC >= kREG(7) + 24.0f) && (thisv->unk_36C == 0)) {
        func_80B57754(thisv, globalCtx);
        func_80B5884C(thisv, globalCtx);
    } else if ((*unk_2EC >= kREG(8) + 50.0f) && (thisv->unk_370 == 0)) {
        func_80B56EB8(thisv, globalCtx);
        thisv->unk_370 = 1;
    } else if ((*unk_2EC >= kREG(9) + 56.0f) && (thisv->unk_374 == 0)) {
        func_80B58898(thisv, globalCtx);
    } else if (*unk_2EC >= kREG(10) + 82.0f) {
        func_80B588E8(thisv, globalCtx);
    }
}

void func_80B58C08(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    Vec3f* unk_348 = &thisv->unk_348;
    Vec3f* unk_354 = &thisv->unk_354;
    Vec3f* thisPos = &thisv->actor.world.pos;
    s32 unk_344;
    s32 unk_346;
    s32 sp28;
    f32 temp_f0;

    thisv->unk_344 += 1;

    unk_344 = thisv->unk_344;
    unk_346 = thisv->unk_346;
    sp28 = unk_346 - kREG(11) - 2;
    temp_f0 = Environment_LerpWeightAccelDecel(unk_346, 0, unk_344, 3, 0);

    thisPos->x = unk_348->x + (temp_f0 * (unk_354->x - unk_348->x));
    thisPos->y = (unk_348->y + (temp_f0 * (unk_354->y - unk_348->y))) + thisv->unk_360;
    thisPos->z = unk_348->z + (temp_f0 * (unk_354->z - unk_348->z));

    if ((unk_344 - sp28) >= 0) {
        thisv->alpha = ((f32)(unk_346 - unk_344) / (kREG(11) + 2)) * 255.0f;
        thisv->drawConfig = 2;
    }

    if ((unk_346 - unk_344) <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80B58D50(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B57EAC(thisv, globalCtx);
}

void func_80B58DB0(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B57EEC(thisv, globalCtx);
}

void func_80B58E10(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, 60.0f);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B57F1C(thisv, globalCtx);
}

void func_80B58E7C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B53764(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B58014(thisv, globalCtx);
    func_80B536B4(thisv);
}

void func_80B58EF4(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B538B0(thisv);
    func_80B53764(thisv, globalCtx);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B58214(thisv, globalCtx);
}

void func_80B58F6C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B536C4(thisv);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B58268(thisv, globalCtx);
}

void func_80B58FDC(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B536C4(thisv);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B582C8(thisv, globalCtx);
}

void func_80B5904C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B536C4(thisv);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B584B4(thisv, globalCtx);
}

void func_80B590BC(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B537E8(thisv);
    func_80B536C4(thisv);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B58624(thisv, globalCtx);
}

void func_80B5912C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B536C4(thisv);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    func_80B56E38(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B57CB4(thisv, globalCtx);
    func_80B57D60(thisv, globalCtx);
    func_80B58A1C(thisv, globalCtx);
}

void func_80B591BC(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B536C4(thisv);
    func_80B538B0(thisv);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B58A50(thisv, globalCtx);
}

void func_80B5922C(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B536C4(thisv);
    func_80B57298(thisv);
    Actor_SetFocus(&thisv->actor, 60.0f);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B58AAC(thisv, globalCtx);
}

void func_80B592A8(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B536C4(thisv);
    func_80B57298(thisv);
    Actor_SetFocus(&thisv->actor, 60.0f);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B57AAC(thisv, EnZl3_UpdateSkelAnime(thisv), &gZelda2Anime2Anim_003D20);
    func_80B56DEC(thisv);
    func_80B58AAC(thisv, globalCtx);
}

void func_80B59340(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B536C4(thisv);
    func_80B57298(thisv);
    Actor_SetFocus(&thisv->actor, 60.0f);
    func_80B533FC(thisv, globalCtx);
    func_80B5366C(thisv, globalCtx);
    EnZl3_UpdateEyes(thisv);
    func_80B57AAC(thisv, EnZl3_UpdateSkelAnime(thisv), &gZelda2Anime2Anim_009FBC);
    func_80B58AAC(thisv, globalCtx);
}

void func_80B593D0(EnZl3* thisv, GlobalContext* globalCtx) {
    func_80B54DE0(thisv, globalCtx);
    func_80B536C4(thisv);
    func_80B57298(thisv);
    func_80B5366C(thisv, globalCtx);
    func_80B56E38(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, 60.0f);
    EnZl3_UpdateEyes(thisv);
    EnZl3_UpdateSkelAnime(thisv);
    func_80B58C08(thisv, globalCtx);
}

s32 func_80B5944C(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                  Gfx** gfx) {
    if (limbIndex == 14) {
        Mtx* mtx = static_cast<Mtx*>(Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Mtx) * 7));
        EnZl3* thisv = (EnZl3*)thisx;
        Vec3s* vec = &thisv->unk_3F8.unk_08;

        gSPSegment(gfx[0]++, 0x0C, mtx);

        rot->x += vec->y;
        rot->z += vec->x;
        Matrix_Push();
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Translate(174.0f, -317.0f, 0.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&mtx[0], "../z_en_zl3_inEscape.c", 2471);
        Matrix_Translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&mtx[1], "../z_en_zl3_inEscape.c", 2474);
        Matrix_Translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&mtx[2], "../z_en_zl3_inEscape.c", 2477);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(40.0f, 264.0f, 386.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&mtx[3], "../z_en_zl3_inEscape.c", 2483);
        Matrix_Translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&mtx[4], "../z_en_zl3_inEscape.c", 2486);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(40.0f, 264.0f, -386.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&mtx[5], "../z_en_zl3_inEscape.c", 2492);
        Matrix_Translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&mtx[6], "../z_en_zl3_inEscape.c", 2495);
        Matrix_Pop();
        Matrix_Pop();
    }
    return false;
}

s32 func_80B59698(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 cond = Flags_GetSwitch(globalCtx, 0x37) &&
               ((globalCtx->sceneNum == SCENE_GANON_DEMO) || (globalCtx->sceneNum == SCENE_GANON_FINAL) ||
                (globalCtx->sceneNum == SCENE_GANON_SONOGO) || (globalCtx->sceneNum == SCENE_GANONTIKA_SONOGO));

    if (cond) {
        u8 curSpawn = globalCtx->curSpawn;

        if ((func_80B54DB4(thisv) == 0x20) && (curSpawn == 0) &&
            ((gSaveContext.timer2Value <= 0) || (gSaveContext.timer2State == 0))) {
            return 1;
        }
    }
    return 0;
}

s32 func_80B59768(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 cond = Flags_GetSwitch(globalCtx, 0x37) &&
               ((globalCtx->sceneNum == SCENE_GANON_DEMO) || (globalCtx->sceneNum == SCENE_GANON_FINAL) ||
                (globalCtx->sceneNum == SCENE_GANON_SONOGO) || (globalCtx->sceneNum == SCENE_GANONTIKA_SONOGO));

    if (cond) {
        u8 curSpawn = globalCtx->curSpawn;

        if ((func_80B54DB4(thisv) == 0x20) && (curSpawn == 0) && (gSaveContext.timer2Value <= 0)) {
            return 1;
        }
    }
    return 0;
}

void func_80B59828(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B59698(thisv, globalCtx) || (!func_80B56EE4(thisv, globalCtx) && func_80B57890(thisv, globalCtx))) {
        s16 newRotY;

        func_80B54E14(thisv, &gZelda2Anime2Anim_009FBC, 0, 0.0f, 0);
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        func_80B56F10(thisv, globalCtx);
        newRotY = func_80B571A8(thisv);
        thisv->actor.shape.rot.y = newRotY;
        thisv->actor.world.rot.y = newRotY;
        thisv->unk_3C4 = thisv->actor.world.rot.z;
        thisv->actor.shape.rot.z = 0;
        thisv->actor.world.rot.z = thisv->actor.shape.rot.z;
        thisv->action = 28;
        thisv->drawConfig = 1;
    } else {
        Actor_Kill(&thisv->actor);
    }

    if (func_80B59698(thisv, globalCtx) != 0) {
        func_80088AA0(180);
        func_80B53468();
        gSaveContext.healthAccumulator = 320;
        Magic_Fill(globalCtx);
        if (Flags_GetSwitch(globalCtx, 0x20)) {
            Flags_UnsetSwitch(globalCtx, 0x20);
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BG_ZG, -144.0f, 3544.0f, -43.0f, 0, 0x2000, 0, 0x2000);
        }
        Flags_UnsetSwitch(globalCtx, 0x21);
        Flags_UnsetSwitch(globalCtx, 0x22);
        Flags_UnsetSwitch(globalCtx, 0x23);
        Flags_UnsetSwitch(globalCtx, 0x24);
        Flags_UnsetSwitch(globalCtx, 0x25);
        Flags_UnsetSwitch(globalCtx, 0x26);
        Flags_UnsetSwitch(globalCtx, 0x27);
        Flags_UnsetSwitch(globalCtx, 0x28);
        Flags_UnsetSwitch(globalCtx, 0x29);
        Flags_UnsetSwitch(globalCtx, 0x2A);
    }

    if (func_80B54DB4(thisv) == 0x20) {
        s32 cond;

        func_80B54EA4(thisv, globalCtx);
        cond = Flags_GetSwitch(globalCtx, 0x37) &&
               ((globalCtx->sceneNum == SCENE_GANON_DEMO) || (globalCtx->sceneNum == SCENE_GANON_FINAL) ||
                (globalCtx->sceneNum == SCENE_GANON_SONOGO) || (globalCtx->sceneNum == SCENE_GANONTIKA_SONOGO));
        if (cond) {
            func_80B53614(thisv, globalCtx);
        }
    }
}

void func_80B59A80(EnZl3* thisv, GlobalContext* globalCtx) {
    if (func_80B59768(thisv, globalCtx)) {
        Audio_PlaySoundGeneral(NA_SE_OC_REVENGE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void func_80B59AD0(EnZl3* thisv, GlobalContext* globalCtx) {
    // todo look into
    Actor* thisx = &thisv->actor; // unused, necessary to use 'thisv' first to fix regalloc

    Flags_SetSwitch(globalCtx, 0x36);
    func_80088AA0(180);
    func_80B54EA4(thisv, globalCtx);
    func_80B53614(thisv, globalCtx);
    gSaveContext.eventChkInf[12] &= ~0x80;
    func_80B56F10(thisv, globalCtx);
    gSaveContext.healthAccumulator = 320;
    Magic_Fill(globalCtx);
    thisv->action = 27;
    thisv->drawConfig = 1;
}

void func_80B59B6C(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 sp2C = func_80B54DD4(thisv);

    thisv->unk_3DC = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_0091D8));
    thisv->unk_3E0 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_00A598));
    thisv->unk_3E4 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_00A334));
    thisv->unk_3F4 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_001110));
    thisv->unk_3EC = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_002348));
    thisv->unk_3F0 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_002E54));
    thisv->unk_3E8 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_001D8C));

    switch (sp2C) {
        case 0:
            func_80B54FB4(thisv, globalCtx);
            break;
        case 1:
            func_80B55780(thisv, globalCtx);
            break;
        case 3:
            func_80B59828(thisv, globalCtx);
            break;
        default:
            osSyncPrintf(VT_FGCOL(RED) " En_Oa3 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            Actor_Kill(&thisv->actor);
    }
}

void func_80B59DB8(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad;
    ObjectContext* objCtx = &globalCtx->objectCtx;
    s32 objIndex = Object_GetIndex(objCtx, OBJECT_ZL2_ANIME2);
    s32 pad2;

    if (objIndex < 0) {
        osSyncPrintf(VT_FGCOL(RED) "En_Zl3_main_bankアニメーションのバンクを読めない!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    if (Object_IsLoaded(objCtx, objIndex)) {
        thisv->unk_318 = objIndex;
        func_80B54DE0(thisv, globalCtx);
        func_80B59B6C(thisv, globalCtx);
    }
}

static EnZl3ActionFunc sActionFuncs[] = {
    func_80B59DB8, func_80B55550, func_80B555A4, func_80B55604, func_80B5566C, func_80B556CC, func_80B5572C,
    func_80B56658, func_80B566AC, func_80B5670C, func_80B5676C, func_80B567CC, func_80B5682C, func_80B568B4,
    func_80B5691C, func_80B5697C, func_80B569E4, func_80B56A68, func_80B56AE0, func_80B56B54, func_80B56BA8,
    func_80B56C24, func_80B56C84, func_80B56CE4, func_80B56D44, func_80B58D50, func_80B58DB0, func_80B58E10,
    func_80B58E7C, func_80B58EF4, func_80B58F6C, func_80B58FDC, func_80B5904C, func_80B590BC, func_80B5912C,
    func_80B591BC, func_80B5922C, func_80B592A8, func_80B59340, func_80B593D0,
};

void EnZl3_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnZl3* thisv = (EnZl3*)thisx;

    if (thisv->action < 0 || thisv->action >= ARRAY_COUNT(sActionFuncs) || sActionFuncs[thisv->action] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[thisv->action](thisv, globalCtx);
}

void EnZl3_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnZl3* thisv = (EnZl3*)thisx;
    ActorShape* shape = &thisv->actor.shape;
    s32 pad;

    osSyncPrintf("ゼルダ姫のEn_Zl3_Actor_ct通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    ActorShape_Init(shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    shape->shadowAlpha = 0;
    func_80B533B0(thisx, globalCtx);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gZelda2Skel, NULL, thisv->jointTable, thisv->morphTable, 15);

    switch (func_80B54DD4(thisv)) {
        case 1:
            gSaveContext.timer2State = 0;
            break;
        case 3:
            func_80B59A80(thisv, globalCtx);
            break;
    }

    osSyncPrintf("ゼルダ姫のEn_Zl3_Actor_ctは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

static OverrideLimbDraw sOverrideLimbDrawFuncs[] = {
    func_80B5458C,
    func_80B5944C,
};

s32 EnZl3_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    EnZl3* thisv = (EnZl3*)thisx;

    if (thisv->unk_308 < 0 || thisv->unk_308 >= ARRAY_COUNT(sOverrideLimbDrawFuncs) ||
        sOverrideLimbDrawFuncs[thisv->unk_308] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画前処理モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return 0;
    }
    return sOverrideLimbDrawFuncs[thisv->unk_308](globalCtx, limbIndex, dList, pos, rot, thisx, gfx);
}

void func_80B59FE8(EnZl3* thisv, GlobalContext* globalCtx) {
}

void func_80B59FF4(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeTexIndex = thisv->eyeTexIndex;
    const void* eyeTex = sEyeTextures[eyeTexIndex];
    s16 mouthTexIndex = thisv->mouthTexIndex;
    SkelAnime* skelAnime = &thisv->skelAnime;
    const void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zl3.c", 2165);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x9, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0xA, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0xB, &D_80116280[2]);

    POLY_OPA_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl3_OverrideLimbDraw, EnZl3_PostLimbDraw, thisv, POLY_OPA_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zl3.c", 2190);
}

void func_80B5A1D0(EnZl3* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeTexIndex = thisv->eyeTexIndex;
    const void* eyeTex = sEyeTextures[eyeTexIndex];
    s16 mouthTexIndex = thisv->mouthTexIndex;
    SkelAnime* skelAnime = &thisv->skelAnime;
    const void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zl3.c", 2205);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 8, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 9, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 10, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
    gSPSegment(POLY_XLU_DISP++, 11, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl3_OverrideLimbDraw, NULL, thisv, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zl3.c", 2234);
}

static EnZl3DrawFunc sDrawFuncs[] = {
    func_80B59FE8,
    func_80B59FF4,
    func_80B5A1D0,
};

void EnZl3_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnZl3* thisv = (EnZl3*)thisx;

    if (thisv->drawConfig < 0 || thisv->drawConfig >= 3 || sDrawFuncs[thisv->drawConfig] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[thisv->drawConfig](thisv, globalCtx);
}

ActorInit En_Zl3_InitVars = {
    ACTOR_EN_ZL3,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZL2,
    sizeof(EnZl3),
    (ActorFunc)EnZl3_Init,
    (ActorFunc)EnZl3_Destroy,
    (ActorFunc)EnZl3_Update,
    (ActorFunc)EnZl3_Draw,
    (ActorResetFunc)EnZl3_Reset,
};

void EnZl3_Reset(void) {
    D_80B5A468 = 0;
    D_80B5A494 = -1;
    D_80B5A4BC = 0;
}