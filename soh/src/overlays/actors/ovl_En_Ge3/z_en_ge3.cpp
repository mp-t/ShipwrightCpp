/*
 * File: z_en_ge3.c
 * Overlay: ovl_En_Ge3
 * Description: Gerudo giving you membership card
 */

#include "z_en_ge3.h"
#include "objects/object_geldb/object_geldb.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnGe3_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGe3_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGe3_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGe3_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnGe3_WaitLookAtPlayer(EnGe3* thisv, GlobalContext* globalCtx);
void EnGe3_ForceTalk(EnGe3* thisv, GlobalContext* globalCtx);
void EnGe3_UpdateWhenNotTalking(Actor* thisx, GlobalContext* globalCtx);

const ActorInit En_Ge3_InitVars = {
    ACTOR_EN_GE3,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GELDB,
    sizeof(EnGe3),
    (ActorFunc)EnGe3_Init,
    (ActorFunc)EnGe3_Destroy,
    (ActorFunc)EnGe3_Update,
    (ActorFunc)EnGe3_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000722, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static EnGe3ActionFunc sActionFuncs[] = { EnGe3_WaitLookAtPlayer };
static AnimationHeader* sAnimations[] = { &gGerudoRedStandAnim }; // Idle with right hand on hip and left over mouth
static u8 sAnimationModes[] = { ANIMMODE_LOOP };

void EnGe3_ChangeAction(EnGe3* thisv, s32 i) {
    thisv->actionFunc = sActionFuncs[i];

    Animation_Change(&thisv->skelAnime, sAnimations[i], 1.0f, 0.0f, (f32)Animation_GetLastFrame(sAnimations[i]),
                     sAnimationModes[i], -8.0f);

    thisv->unk_30C &= ~2;
}

void EnGe3_Init(Actor* thisx, GlobalContext* globalCtx2) {
    EnGe3* thisv = (EnGe3*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGerudoRedSkel, NULL, thisv->jointTable, thisv->morphTable,
                       GELDB_LIMB_MAX);
    Animation_PlayLoop(&thisv->skelAnime, &gGerudoRedStandAnim);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.world.rot.z = 0;
    thisv->actor.shape.rot.z = 0;
    EnGe3_ChangeAction(thisv, 0);
    thisv->actionFunc = EnGe3_ForceTalk;
    thisv->unk_30C = 0;
    thisv->actor.targetMode = 6;
    thisv->actor.minVelocityY = -4.0f;
    thisv->actor.gravity = -1.0f;
}

void EnGe3_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGe3* thisv = (EnGe3*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnGe3_TurnToFacePlayer(EnGe3* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 angleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 6, 4000, 100);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_306, thisv->actor.focus.pos);
    } else {
        if (angleDiff < 0) {
            Math_SmoothStepToS(&thisv->headRot.y, -0x2000, 6, 6200, 0x100);
        } else {
            Math_SmoothStepToS(&thisv->headRot.y, 0x2000, 6, 6200, 0x100);
        }

        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 12, 1000, 100);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    }
}

void EnGe3_LookAtPlayer(EnGe3* thisv, GlobalContext* globalCtx) {
    if ((ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x2300) &&
        (thisv->actor.xzDistToPlayer < 100.0f)) {
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_306, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->headRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->headRot.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_306.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_306.y, 0, 6, 6200, 100);
    }
}

void EnGe3_Wait(EnGe3* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe3_WaitLookAtPlayer;
        thisv->actor.update = EnGe3_UpdateWhenNotTalking;
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    }
    EnGe3_TurnToFacePlayer(thisv, globalCtx);
}

void EnGe3_WaitLookAtPlayer(EnGe3* thisv, GlobalContext* globalCtx) {
    EnGe3_LookAtPlayer(thisv, globalCtx);
}

void EnGe3_WaitTillCardGiven(EnGe3* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnGe3_Wait;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_GERUDO_CARD, 10000.0f, 50.0f);
    }
}

void EnGe3_GiveCard(EnGe3* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        thisv->actionFunc = EnGe3_WaitTillCardGiven;
        func_8002F434(&thisv->actor, globalCtx, GI_GERUDO_CARD, 10000.0f, 50.0f);
    }
}

void EnGe3_ForceTalk(EnGe3* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe3_GiveCard;
    } else {
        if (!(thisv->unk_30C & 4)) {
            func_8002DF54(globalCtx, &thisv->actor, 7);
            thisv->unk_30C |= 4;
        }
        thisv->actor.textId = 0x6004;
        thisv->actor.flags |= ACTOR_FLAG_16;
        func_8002F1C4(&thisv->actor, globalCtx, 300.0f, 300.0f, 0);
    }
    EnGe3_LookAtPlayer(thisv, globalCtx);
}

void EnGe3_UpdateCollision(EnGe3* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 pad2;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 40.0f, 25.0f, 40.0f, 5);

    if (!(thisv->unk_30C & 2) && SkelAnime_Update(&thisv->skelAnime)) {
        thisv->unk_30C |= 2;
    }
}

void EnGe3_MoveAndBlink(EnGe3* thisv, GlobalContext* globalCtx) {

    Actor_MoveForward(&thisv->actor);

    if (DECR(thisv->blinkTimer) == 0) {
        thisv->blinkTimer = Rand_S16Offset(60, 60);
    }

    thisv->eyeIndex = thisv->blinkTimer;

    if (thisv->eyeIndex >= 3) {
        thisv->eyeIndex = 0;
    }
}

void EnGe3_UpdateWhenNotTalking(Actor* thisx, GlobalContext* globalCtx) {
    EnGe3* thisv = (EnGe3*)thisx;

    EnGe3_UpdateCollision(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe3_Wait;
        thisv->actor.update = EnGe3_Update;
    } else {
        thisv->actor.textId = 0x6005;
        if (thisv->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
    }

    EnGe3_MoveAndBlink(thisv, globalCtx);
}

void EnGe3_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGe3* thisv = (EnGe3*)thisx;

    EnGe3_UpdateCollision(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    EnGe3_MoveAndBlink(thisv, globalCtx);
}

s32 EnGe3_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGe3* thisv = (EnGe3*)thisx;

    switch (limbIndex) {
        // Hide swords and veil from object_geldb
        case GELDB_LIMB_VEIL:
        case GELDB_LIMB_R_SWORD:
        case GELDB_LIMB_L_SWORD:
            *dList = NULL;
            return false;
        // Turn head
        case GELDB_LIMB_HEAD:
            rot->x += thisv->headRot.y;

        // This is a hack to fix the color-changing clothes thisv Gerudo has on N64 versions
        default:
            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ge3.c", 547);
            switch (limbIndex) {
                case GELDB_LIMB_NECK:
                    break;
                case GELDB_LIMB_HEAD:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 80, 60, 10, 255);
                    break;
                case GELDB_LIMB_R_SWORD:
                case GELDB_LIMB_L_SWORD:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 140, 170, 230, 255);
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                    break;
                default:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 140, 0, 0, 255);
                    break;
            }
            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ge3.c", 566);
            break;
    }
    return false;
}

void EnGe3_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGe3* thisv = (EnGe3*)thisx;
    Vec3f D_80A351C8 = { 600.0f, 700.0f, 0.0f };

    if (limbIndex == GELDB_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80A351C8, &thisv->actor.focus.pos);
    }
}

void EnGe3_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    static void* eyeTextures[] = {
        gGerudoRedEyeOpenTex,
        gGerudoRedEyeHalfTex,
        gGerudoRedEyeShutTex,
    };
    EnGe3* thisv = (EnGe3*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ge3.c", 614);

    func_800943C8(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeIndex]));
    func_8002EBCC(&thisv->actor, globalCtx, 0);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnGe3_OverrideLimbDraw, EnGe3_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ge3.c", 631);
}
