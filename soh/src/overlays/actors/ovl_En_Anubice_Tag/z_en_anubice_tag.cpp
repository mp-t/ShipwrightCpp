/*
 * File: z_en_anubice_tag.c
 * Overlay: ovl_En_Anubice_Tag
 * Description: Spawns and manages the Anubis enemy
 */

#include "z_en_anubice_tag.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnAnubiceTag_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAnubiceTag_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAnubiceTag_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAnubiceTag_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnAnubiceTag_SpawnAnubis(EnAnubiceTag* thisv, GlobalContext* globalCtx);
void EnAnubiceTag_ManageAnubis(EnAnubiceTag* thisv, GlobalContext* globalCtx);

ActorInit En_Anubice_Tag_InitVars = {
    ACTOR_EN_ANUBICE_TAG,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnAnubiceTag),
    (ActorFunc)EnAnubiceTag_Init,
    (ActorFunc)EnAnubiceTag_Destroy,
    (ActorFunc)EnAnubiceTag_Update,
    (ActorFunc)EnAnubiceTag_Draw,
    NULL,
};

void EnAnubiceTag_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubiceTag* thisv = (EnAnubiceTag*)thisx;

    osSyncPrintf("\n\n");
    // "Anubis control tag generated"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ アヌビス制御タグ発生 ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params);

    if (thisv->actor.params < -1) {
        thisv->actor.params = 0;
    }
    if (thisv->actor.params != 0) {
        thisv->triggerRange = thisv->actor.params * 40.0f;
    }
    thisv->actionFunc = EnAnubiceTag_SpawnAnubis;
}

void EnAnubiceTag_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnAnubiceTag_SpawnAnubis(EnAnubiceTag* thisv, GlobalContext* globalCtx) {
    thisv->anubis = (EnAnubice*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_ANUBICE,
                                                  thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                                  thisv->actor.world.pos.z, 0, thisv->actor.yawTowardsPlayer, 0, 0);

    if (thisv->anubis != NULL) {
        thisv->actionFunc = EnAnubiceTag_ManageAnubis;
    }
}

void EnAnubiceTag_ManageAnubis(EnAnubiceTag* thisv, GlobalContext* globalCtx) {
    EnAnubice* anubis;
    Vec3f offset;

    if (thisv->anubis != NULL) {
        anubis = thisv->anubis;
        if (anubis->actor.update == NULL) {
            return;
        }
    } else {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (anubis->deathTimer != 0) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->actor.xzDistToPlayer < (200.0f + thisv->triggerRange)) {
        if (!anubis->isLinkOutOfRange) {
            if (!anubis->isKnockedback) {
                anubis->isMirroringLink = true;
                offset.x = -Math_SinS(thisv->actor.yawTowardsPlayer) * thisv->actor.xzDistToPlayer;
                offset.z = -Math_CosS(thisv->actor.yawTowardsPlayer) * thisv->actor.xzDistToPlayer;
                Math_ApproachF(&anubis->actor.world.pos.x, (thisv->actor.world.pos.x + offset.x), 0.3f, 10.0f);
                Math_ApproachF(&anubis->actor.world.pos.z, (thisv->actor.world.pos.z + offset.z), 0.3f, 10.0f);
                return;
            }
        }
    } else {
        if (anubis->isMirroringLink) {
            anubis->isLinkOutOfRange = true;
        }
    }
}

void EnAnubiceTag_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubiceTag* thisv = (EnAnubiceTag*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void EnAnubiceTag_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubiceTag* thisv = (EnAnubiceTag*)thisx;

    if (BREG(0) != 0) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                               thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0xFF, 0, 0, 0xFF, 4, globalCtx->state.gfxCtx);
    }
}
