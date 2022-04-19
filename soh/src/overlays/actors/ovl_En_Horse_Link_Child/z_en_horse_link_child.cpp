/*
 * File: z_en_horse_link_child.c
 * Overlay: ovl_En_Horse_Link_Child
 * Description: Young Epona
 */

#include "z_en_horse_link_child.h"
#include "objects/object_horse_link_child/object_horse_link_child.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnHorseLinkChild_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHorseLinkChild_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHorseLinkChild_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHorseLinkChild_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A69B7C(EnHorseLinkChild* thisv);
void func_80A69EC0(EnHorseLinkChild* thisv);
void func_80A6A4DC(EnHorseLinkChild* thisv);
void func_80A6A724(EnHorseLinkChild* thisv);

ActorInit En_Horse_Link_Child_InitVars = {
    ACTOR_EN_HORSE_LINK_CHILD,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE_LINK_CHILD,
    sizeof(EnHorseLinkChild),
    (ActorFunc)EnHorseLinkChild_Init,
    (ActorFunc)EnHorseLinkChild_Destroy,
    (ActorFunc)EnHorseLinkChild_Update,
    (ActorFunc)EnHorseLinkChild_Draw,
    NULL,
};

static const AnimationHeader* sAnimations[] = {
    &gChildEponaIdleAnim,     &gChildEponaWhinnyAnim,    &gChildEponaWalkingAnim,
    &gChildEponaTrottingAnim, &gChildEponaGallopingAnim,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
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
    { 20, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementInit,
};

static CollisionCheckInfoInit sColCheckInfoInit = { 10, 35, 100, MASS_HEAVY };

void func_80A693D0(EnHorseLinkChild* thisv) {
    static s32 D_80A6AF5C[] = { 1, 19 };

    if ((thisv->skin.skelAnime.curFrame > D_80A6AF5C[thisv->unk_1F0]) &&
        !((thisv->unk_1F0 == 0) && (thisv->skin.skelAnime.curFrame > D_80A6AF5C[1]))) {
        Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_WALK, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        thisv->unk_1F0++;
        if (thisv->unk_1F0 >= ARRAY_COUNT(D_80A6AF5C)) {
            thisv->unk_1F0 = 0;
        }
    }
}

void func_80A6948C(EnHorseLinkChild* thisv) {
    if (thisv->animationIdx == 2) {
        func_80A693D0(thisv);
    } else if (thisv->skin.skelAnime.curFrame == 0.0f) {
        if ((thisv->animationIdx == 3) || (thisv->animationIdx == 4)) {
            Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        } else if (thisv->animationIdx == 1) {
            if (Rand_ZeroOne() > 0.5f) {
                Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_GROAN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            } else {
                Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            }
        }
    }
}

static f32 D_80A6AF64[] = { 1.0f, 1.0f, 1.5f, 1.5f, 1.5f };

f32 func_80A695A4(EnHorseLinkChild* thisv) {
    f32 result;

    if (thisv->animationIdx == 2) {
        result = D_80A6AF64[thisv->animationIdx] * thisv->actor.speedXZ * (1.0f / 2.0f);
    } else if (thisv->animationIdx == 3) {
        result = D_80A6AF64[thisv->animationIdx] * thisv->actor.speedXZ * (1.0f / 3.0f);
    } else if (thisv->animationIdx == 4) {
        result = D_80A6AF64[thisv->animationIdx] * thisv->actor.speedXZ * (1.0f / 5.0f);
    } else {
        result = D_80A6AF64[thisv->animationIdx];
    }

    return result;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_STOP),
};

void EnHorseLinkChild_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseLinkChild* thisv = (EnHorseLinkChild*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 0.005f);
    thisv->actor.gravity = -3.5f;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->action = 1;
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;
    Skin_Init(globalCtx, &thisv->skin, &gChildEponaSkel, &gChildEponaGallopingAnim);
    thisv->animationIdx = 0;
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[0]);
    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinderType1(globalCtx, &thisv->bodyCollider, &thisv->actor, &sCylinderInit);
    Collider_InitJntSph(globalCtx, &thisv->headCollider);
    Collider_SetJntSph(globalCtx, &thisv->headCollider, &thisv->actor, &sJntSphInit, thisv->headElements);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColCheckInfoInit);
    thisv->unk_1F0 = 0;
    thisv->eyeTexIndex = 0;

    if (gSaveContext.sceneSetupIndex > 3) {
        func_80A69EC0(thisv);
    } else if (globalCtx->sceneNum == SCENE_SPOT20) {
        if (!Flags_GetEventChkInf(0x14)) {
            Actor_Kill(&thisv->actor);
            return;
        }
        thisv->unk_2A0 = gSaveContext.eventChkInf[1] & 0x40;
        func_80A69EC0(thisv);
    } else {
        func_80A69EC0(thisv);
    }

    thisv->actor.home.rot.z = thisv->actor.world.rot.z = thisv->actor.shape.rot.z = 0;
}

void EnHorseLinkChild_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseLinkChild* thisv = (EnHorseLinkChild*)thisx;

    Skin_Free(globalCtx, &thisv->skin);
    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
    Collider_DestroyJntSph(globalCtx, &thisv->headCollider);
}

void func_80A6988C(EnHorseLinkChild* thisv) {
    thisv->action = 0;
    thisv->animationIdx++;
    if (thisv->animationIdx >= ARRAY_COUNT(sAnimations)) {
        thisv->animationIdx = 0;
    }

    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx]);
    thisv->skin.skelAnime.playSpeed = func_80A695A4(thisv);
}

void func_80A698F4(EnHorseLinkChild* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        func_80A6988C(thisv);
    }
}

void func_80A6993C(EnHorseLinkChild* thisv, s32 newAnimationIdx) {
    thisv->action = 2;
    thisv->actor.speedXZ = 0.0f;

    if (!((newAnimationIdx == 0) || (newAnimationIdx == 1))) {
        newAnimationIdx = 0;
    }

    if (thisv->animationIdx != newAnimationIdx) {
        thisv->animationIdx = newAnimationIdx;
        Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                         Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
    }
}

void func_80A699FC(EnHorseLinkChild* thisv, GlobalContext* globalCtx) {
    f32 distFromLink;
    s32 newAnimationIdx;

    distFromLink = Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        if ((distFromLink < 1000.0f) && (distFromLink > 70.0f)) {
            func_80A69B7C(thisv);
        } else {
            newAnimationIdx = thisv->animationIdx == 1 ? 0 : 1;
            if (thisv->animationIdx != newAnimationIdx) {
                thisv->animationIdx = newAnimationIdx;
                Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                                 Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
            } else {
                Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                                 Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0);
            }
        }
    }
}

void func_80A69B7C(EnHorseLinkChild* thisv) {
    thisv->action = 1;
    thisv->animationIdx = 0;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void func_80A69C18(EnHorseLinkChild* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;
    f32 distFromLink;
    s32 newAnimationIdx;

    if ((thisv->animationIdx == 4) || (thisv->animationIdx == 3) || (thisv->animationIdx == 2)) {
        yawDiff = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.world.rot.y;

        if (yawDiff > 0x12C) {
            thisv->actor.world.rot.y += 0x12C;
        } else if (yawDiff < -0x12C) {
            thisv->actor.world.rot.y -= 0x12C;
        } else {
            thisv->actor.world.rot.y += yawDiff;
        }

        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        distFromLink = Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);

        if (distFromLink > 1000.0f) {
            func_80A6993C(thisv, 0);
            return;
        } else if ((distFromLink < 1000.0f) && (distFromLink >= 300.0f)) {
            newAnimationIdx = 4;
            thisv->actor.speedXZ = 6.0f;
        } else if ((distFromLink < 300.0f) && (distFromLink >= 150.0f)) {
            newAnimationIdx = 3;
            thisv->actor.speedXZ = 4.0f;
        } else if ((distFromLink < 150.0f) && (distFromLink >= 70.0f)) {
            newAnimationIdx = 2;
            thisv->actor.speedXZ = 2.0f;
            thisv->unk_1F0 = 0;
        } else {
            func_80A6993C(thisv, 1);
            return;
        }

        if (thisv->animationIdx != newAnimationIdx) {
            thisv->animationIdx = newAnimationIdx;
            Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                             Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
        } else {
            Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                             Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

void func_80A69EC0(EnHorseLinkChild* thisv) {
    thisv->action = 3;
    thisv->animationIdx = 0;
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void func_80A69F5C(EnHorseLinkChild* thisv, GlobalContext* globalCtx) {
    Player* player;
    s16 yawDiff;
    s32 yawSign;
    s32 yawOffset;

    if ((thisv->animationIdx == 4) || (thisv->animationIdx == 3) || (thisv->animationIdx == 2)) {
        player = GET_PLAYER(globalCtx);

        if (Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &thisv->actor.home.pos) < 250.0f) {
            yawDiff = player->actor.shape.rot.y;
            yawSign = Actor_WorldYawTowardActor(&thisv->actor, &player->actor) > 0 ? 1 : -1;
            yawOffset = yawSign << 0xE;
            yawDiff += yawOffset;
        } else {
            yawDiff = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos) - thisv->actor.world.rot.y;
        }

        if (yawDiff > 0x12C) {
            thisv->actor.world.rot.y += 0x12C;
        } else if (yawDiff < -0x12C) {
            thisv->actor.world.rot.y -= 0x12C;
        } else {
            thisv->actor.world.rot.y += yawDiff;
        }

        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    }
}

void func_80A6A068(EnHorseLinkChild* thisv, GlobalContext* globalCtx) {
    Player* player;
    f32 distFromLink;
    s32 animationEnded;
    s32 newAnimationIdx;
    f32 distFromHome;
    f32 distLinkFromHome;

    func_80A69F5C(thisv, globalCtx);
    player = GET_PLAYER(globalCtx);
    distFromLink = Actor_WorldDistXZToActor(&thisv->actor, &player->actor);

    if (gSaveContext.entranceIndex == 0x2AE) {
        Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_80A6A724(thisv);
        return;
    }

    if (((gSaveContext.eventChkInf[1] & 0x40) && (DREG(53) != 0)) ||
        ((globalCtx->sceneNum == SCENE_SPOT20) && (gSaveContext.cutsceneIndex == 0xFFF1))) {
        func_80A6A4DC(thisv);
    } else {
        thisv->unk_2A0 = gSaveContext.eventChkInf[1] & 0x40;
    }

    newAnimationIdx = thisv->animationIdx;
    animationEnded = SkelAnime_Update(&thisv->skin.skelAnime);
    if (animationEnded || (thisv->animationIdx == 1) || (thisv->animationIdx == 0)) {
        if (gSaveContext.eventChkInf[1] & 0x20) {
            distFromHome = Math3D_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->actor.home.pos);
            distLinkFromHome = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &thisv->actor.home.pos);
            if (distLinkFromHome > 250.0f) {
                if (distFromHome >= 300.0f) {
                    newAnimationIdx = 4;
                    thisv->actor.speedXZ = 6.0f;
                } else if ((distFromHome < 300.0f) && (distFromHome >= 150.0f)) {
                    newAnimationIdx = 3;
                    thisv->actor.speedXZ = 4.0f;
                } else if ((distFromHome < 150.0f) && (distFromHome >= 70.0f)) {
                    newAnimationIdx = 2;
                    thisv->actor.speedXZ = 2.0f;
                    thisv->unk_1F0 = 0;
                } else {
                    thisv->actor.speedXZ = 0.0f;
                    if (thisv->animationIdx == 0) {
                        newAnimationIdx = animationEnded == true ? 1 : 0;
                    } else {
                        newAnimationIdx = animationEnded == true ? 0 : 1;
                    }
                }
            } else {
                if (distFromLink < 200.0f) {
                    newAnimationIdx = 4;
                    thisv->actor.speedXZ = 6.0f;
                } else if (distFromLink < 300.0f) {
                    newAnimationIdx = 3;
                    thisv->actor.speedXZ = 4.0f;
                } else if (distFromLink < 400.0f) {
                    newAnimationIdx = 2;
                    thisv->actor.speedXZ = 2.0f;
                    thisv->unk_1F0 = 0;
                } else {
                    thisv->actor.speedXZ = 0.0f;
                    if (thisv->animationIdx == 0) {
                        newAnimationIdx = animationEnded == true ? 1 : 0;
                    } else {
                        newAnimationIdx = animationEnded == true ? 0 : 1;
                    }
                }
            }
        } else {
            thisv->actor.speedXZ = 0.0f;
            if (thisv->animationIdx == 0) {
                newAnimationIdx = animationEnded == true ? 1 : 0;
            } else {
                newAnimationIdx = animationEnded == true ? 0 : 1;
            }
        }
    }

    if ((thisv->animationIdx != newAnimationIdx) || (animationEnded == true)) {
        thisv->animationIdx = newAnimationIdx;
        Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                         Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
    } else {
        Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv),
                         thisv->skin.skelAnime.curFrame, Animation_GetLastFrame(sAnimations[thisv->animationIdx]), 2,
                         0.0f);
    }
}

void func_80A6A4DC(EnHorseLinkChild* thisv) {
    thisv->action = 5;
    thisv->animationIdx = Rand_ZeroOne() > 0.5f ? 0 : 1;
    DREG(53) = 0;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void func_80A6A5A4(EnHorseLinkChild* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;

    if (DREG(53) != 0) {
        DREG(53) = 0;
        Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_80A6A724(thisv);
    } else {
        thisv->actor.speedXZ = 0.0f;
        yawDiff = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.world.rot.y;
        // 0.7071 = cos(pi/4)
        if ((Math_CosS(yawDiff) < 0.7071f) && (thisv->animationIdx == 2)) {
            func_8006DD9C(&thisv->actor, &GET_PLAYER(globalCtx)->actor.world.pos, 300);
        }

        if (SkelAnime_Update(&thisv->skin.skelAnime)) {
            if (Math_CosS(yawDiff) < 0.0f) {
                thisv->animationIdx = 2;
                Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], D_80A6AF64[thisv->animationIdx],
                                 0.0f, Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
            } else {
                func_80A6A4DC(thisv);
            }
        }
    }
}

void func_80A6A724(EnHorseLinkChild* thisv) {
    thisv->timer = 0;
    thisv->action = 4;
    thisv->animationIdx = 2;
    thisv->unk_1E8 = false;
    thisv->actor.speedXZ = 2.0f;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void func_80A6A7D0(EnHorseLinkChild* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 dist;
    s32 newAnimationIdx;

    thisv->timer++;
    if (thisv->timer > 300) {
        thisv->unk_1E8 = true;
    }

    if ((thisv->animationIdx == 4) || (thisv->animationIdx == 3) || (thisv->animationIdx == 2)) {
        if (!thisv->unk_1E8) {
            func_8006DD9C(&thisv->actor, &player->actor.world.pos, 300);
        } else {
            func_8006DD9C(&thisv->actor, &thisv->actor.home.pos, 300);
        }
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        if (!thisv->unk_1E8) {
            dist = Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);
        } else {
            dist = Math3D_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->actor.home.pos);
        }

        if (!thisv->unk_1E8) {
            if (dist >= 300.0f) {
                newAnimationIdx = 4;
                thisv->actor.speedXZ = 6.0f;
            } else if (dist >= 150.0f) {
                newAnimationIdx = 3;
                thisv->actor.speedXZ = 4.0f;
            } else {
                newAnimationIdx = 2;
                thisv->actor.speedXZ = 2.0f;
                thisv->unk_1F0 = 0;
            }
        } else {
            if (dist >= 300.0f) {
                newAnimationIdx = 4;
                thisv->actor.speedXZ = 6.0f;
            } else if (dist >= 150.0f) {
                newAnimationIdx = 3;
                thisv->actor.speedXZ = 4.0f;
            } else if (dist >= 70.0f) {
                newAnimationIdx = 2;
                thisv->actor.speedXZ = 2.0f;
                thisv->unk_1F0 = 0;
            } else {
                func_80A6A4DC(thisv);
                return;
            }
        }

        if (thisv->animationIdx != newAnimationIdx) {
            thisv->animationIdx = newAnimationIdx;
            Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                             Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -5.0f);
        } else {
            Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A695A4(thisv), 0.0f,
                             Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

static EnHorseLinkChildActionFunc sActionFuncs[] = {
    func_80A698F4, func_80A69C18, func_80A699FC, func_80A6A068, func_80A6A7D0, func_80A6A5A4,
};

static const void* sEyeTextures[] = { gChildEponaEyeOpenTex, gChildEponaEyeHalfTex, gChildEponaEyeCloseTex };
static u8 sEyeIndexOrder[] = { 0, 1, 2, 1 };

void EnHorseLinkChild_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseLinkChild* thisv = (EnHorseLinkChild*)thisx;
    s32 pad;

    sActionFuncs[thisv->action](thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 55.0f, 100.0f, 0x1D);

    if ((globalCtx->sceneNum == SCENE_SPOT20) && (thisv->actor.world.pos.z < -2400.0f)) {
        thisv->actor.world.pos.z = -2400.0f;
    }

    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;

    if ((Rand_ZeroOne() < 0.025f) && (thisv->eyeTexIndex == 0)) {
        thisv->eyeTexIndex++;
    } else if (thisv->eyeTexIndex > 0) {
        thisv->eyeTexIndex++;
        if (thisv->eyeTexIndex >= ARRAY_COUNT(sEyeIndexOrder)) {
            thisv->eyeTexIndex = 0;
        }
    }

    Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
    func_80A6948C(thisv);
}

void EnHorseLinkChild_PostDraw(Actor* thisx, GlobalContext* globalCtx, Skin* skin) {
    Vec3f center;
    Vec3f newCenter;
    EnHorseLinkChild* thisv = (EnHorseLinkChild*)thisx;
    s32 i;

    for (i = 0; i < thisv->headCollider.count; i++) {
        center.x = thisv->headCollider.elements[i].dim.modelSphere.center.x;
        center.y = thisv->headCollider.elements[i].dim.modelSphere.center.y;
        center.z = thisv->headCollider.elements[i].dim.modelSphere.center.z;
        Skin_GetLimbPos(skin, thisv->headCollider.elements[i].dim.limb, &center, &newCenter);
        thisv->headCollider.elements[i].dim.worldSphere.center.x = newCenter.x;
        thisv->headCollider.elements[i].dim.worldSphere.center.y = newCenter.y;
        thisv->headCollider.elements[i].dim.worldSphere.center.z = newCenter.z;
        thisv->headCollider.elements[i].dim.worldSphere.radius =
            thisv->headCollider.elements[i].dim.modelSphere.radius * thisv->headCollider.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->headCollider.base);
}

s32 EnHorseLinkChild_OverrideLimbDraw(Actor* thisx, GlobalContext* globalCtx, s32 arg2, Skin* skin) {
    EnHorseLinkChild* thisv = (EnHorseLinkChild*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_horse_link_child.c", 1467);

    if (arg2 == 0xD) {
        u8 index = sEyeIndexOrder[thisv->eyeTexIndex];

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[index]));
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_horse_link_child.c", 1479);

    return 1;
}

void EnHorseLinkChild_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseLinkChild* thisv = (EnHorseLinkChild*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    func_800A6360(&thisv->actor, globalCtx, &thisv->skin, EnHorseLinkChild_PostDraw, EnHorseLinkChild_OverrideLimbDraw,
                  true);
}
