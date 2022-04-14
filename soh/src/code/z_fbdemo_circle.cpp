#include "global.h"
#include <string.h>

// unused
Gfx sCircleNullDList[] = {
    gsSPEndDisplayList(),
};

//#include "code/fbdemo_circle/z_fbdemo_circle.c"
#include "code/fbdemo_circle/z_fbdemo_circle.h"

Gfx __sCircleDList[] = {
    gsDPPipeSync(),                                                                                                 // 0
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |                  // 1
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),                                                                // 2
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |    // 3
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),                                      // 4
    gsDPSetCombineMode(G_CC_BLENDPEDECALA, G_CC_BLENDPEDECALA),                                                     // 5
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),                                                          // 6
    gsDPLoadTextureBlock(0xF8000000, G_IM_FMT_I, G_IM_SIZ_8b, 16, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,                 // 7
                         G_TX_NOMIRROR | G_TX_CLAMP, 4, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPDisplayList(0xF9000000),                                                                                    // 8
    gsSPVertex(sCircleWipeVtx, 32, 0),                                                                              // 9
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),                                                                         // 10
    gsSP2Triangles(3, 5, 6, 0, 5, 7, 8, 0),                                                                         // 11
    gsSP2Triangles(7, 9, 10, 0, 9, 11, 12, 0),                                                                      // 12
    gsSP2Triangles(11, 13, 14, 0, 13, 15, 16, 0),                                                                   // 13
    gsSP2Triangles(15, 17, 18, 0, 17, 19, 20, 0),                                                                   // 14
    gsSP2Triangles(19, 21, 22, 0, 21, 23, 24, 0),                                                                   // 15
    gsSP2Triangles(23, 25, 26, 0, 25, 27, 28, 0),                                                                   // 16
    gsSP1Triangle(27, 29, 30, 0),                                                                                   // 17
    gsSPVertex(&sCircleWipeVtx[31], 3, 0),                                                                          // 18
    gsSP1Triangle(0, 1, 2, 0),                                                                                      // 19
    gsSPEndDisplayList(),                                                                                           // 20
};

void TransitionCircle_Start(void* thisx) {
    TransitionCircle* thisv = (TransitionCircle*)thisx;

    thisv->isDone = 0;

    switch (thisv->effect) {
        case 1:
            thisv->texture = sCircleWipeWaveTex;
            break;
        case 2:
            thisv->texture = sCircleWipeRippleTex;
            break;
        case 3:
            thisv->texture = sCircleWipeStarburstTex;
            break;
        default:
            thisv->texture = sCircleWipeDefaultTex;
            break;
    }

    thisv->texture = ResourceMgr_LoadTexByName(thisv->texture);

    if (thisv->speed == 0) {
        thisv->step = 0x14;
    } else {
        thisv->step = 0xA;
    }

    if (thisv->typeColor == 0) {
        thisv->color.rgba = RGBA8(0, 0, 0, 255);
    } else if (thisv->typeColor == 1) {
        thisv->color.rgba = RGBA8(160, 160, 160, 255);
    } else if (thisv->typeColor == 2) {
        // yes, really.
        thisv->color.r = 100;
        thisv->color.g = 100;
        thisv->color.b = 100;
        thisv->color.a = 255;
    } else {
        thisv->step = 0x28;
        thisv->color.rgba = thisv->effect == 1 ? RGBA8(0, 0, 0, 255) : RGBA8(160, 160, 160, 255);
    }
    if (thisv->unk_14 != 0) {
        thisv->texY = 0;
        if (thisv->typeColor == 3) {
            thisv->texY = 0xFA;
        }
    } else {
        thisv->texY = 0x1F4;
        if (thisv->effect == 2) {
            Audio_PlaySoundGeneral(NA_SE_OC_SECRET_WARP_OUT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
    guPerspective(&thisv->projection, &thisv->normal, 60.0f, (4.0f / 3.0f), 10.0f, 12800.0f, 1.0f);
    guLookAt(&thisv->lookAt, 0.0f, 0.0f, 400.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void* TransitionCircle_Init(void* thisx) {
    TransitionCircle* thisv = (TransitionCircle*)thisx;

    memset(thisv, 0, sizeof(*thisv));
    return thisv;
}

void TransitionCircle_Destroy(void* thisx) {
}

void TransitionCircle_Update(void* thisx, s32 updateRate) {
    TransitionCircle* thisv = (TransitionCircle*)thisx;
    s32 temp_t2;
    s32 temp_t3;

    if (thisv->unk_14 != 0) {
        if (thisv->texY == 0) {
            if (thisv->effect == 2) {
                Audio_PlaySoundGeneral(NA_SE_OC_SECRET_WARP_IN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
        thisv->texY += thisv->step * 3 / updateRate;
        if (thisv->texY >= 0x1F4) {
            thisv->texY = 0x1F4;
            thisv->isDone = 1;
        }
    } else {
        thisv->texY -= thisv->step * 3 / updateRate;
        if (thisv->typeColor != 3) {
            if (thisv->texY <= 0) {
                thisv->texY = 0;
                thisv->isDone = 1;
            }
        } else {
            if (thisv->texY < 0xFB) {
                thisv->texY = 0xFA;
                thisv->isDone = 1;
            }
        }
    }
}

void TransitionCircle_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    TransitionCircle* thisv = (TransitionCircle*)thisx;
    Gfx* texScroll;
    // These variables are a best guess based on the other transition types.
    f32 tPos = 0.0f;
    f32 rot = 0.0f;
    f32 scale = 14.8f;

    modelView = thisv->modelView[thisv->frame];

    thisv->color.rgba = 0xFFFFFFFF;

    thisv->frame ^= 1;
    gDPPipeSync(gfx++);
    texScroll = Gfx_BranchTexScroll(&gfx, thisv->texX, thisv->texY, 0x10, 0x40);
    gSPSegment(gfx++, 9, texScroll);
    gSPSegment(gfx++, 8, thisv->texture);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, thisv->color.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, thisv->color.rgba);
    gSPMatrix(gfx++, &thisv->projection, G_MTX_PROJECTION | G_MTX_LOAD);
    gSPPerspNormalize(gfx++, thisv->normal);
    gSPMatrix(gfx++, &thisv->lookAt, G_MTX_PROJECTION | G_MTX_NOPUSH | G_MTX_MUL);

    float aspectRatio = OTRGetAspectRatio();

    if (scale != 1.0f) {
        guScale(&modelView[0], scale * aspectRatio, scale * aspectRatio, 1.0f);
        gSPMatrix(gfx++, &modelView[0], G_MTX_LOAD);
    }

    if (rot != 0.0f) {
        guRotate(&modelView[1], rot, 0.0f, 0.0f, 1.0f);
        gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    }

    if ((tPos != 0.0f) || (tPos != 0.0f)) {
        guTranslate(&modelView[2], tPos, tPos, 0.0f);
        gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    }

    // OTRTODO: This is an ugly hack but it will do for now...
    Vtx* vtx = ResourceMgr_LoadVtxByName(sCircleWipeVtx);
    Gfx var1 = gsSPVertex(vtx, 32, 0);
    Gfx var2 = gsSPVertex(&vtx[31], 3, 0);
    __sCircleDList[0xe] = var1;
    __sCircleDList[0x17] = var2;

    gSPDisplayList(gfx++, __sCircleDList);
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionCircle_IsDone(void* thisx) {
    TransitionCircle* thisv = (TransitionCircle*)thisx;

    return thisv->isDone;
}

void TransitionCircle_SetType(void* thisx, s32 type) {
    TransitionCircle* thisv = (TransitionCircle*)thisx;

    if (type & 0x80) {
        thisv->unk_14 = (type >> 5) & 0x1;
        thisv->typeColor = (type >> 3) & 0x3;
        thisv->speed = type & 0x1;
        thisv->effect = (type >> 1) & 0x3;
    } else if (type == 1) {
        thisv->unk_14 = 1;
    } else {
        thisv->unk_14 = 0;
    }
}

void TransitionCircle_SetColor(void* thisx, u32 color) {
    TransitionCircle* thisv = (TransitionCircle*)thisx;

    thisv->color.rgba = color;
}

void TransitionCircle_SetEnvColor(void* thisx, u32 envColor) {
    TransitionCircle* thisv = (TransitionCircle*)thisx;

    thisv->envColor.rgba = envColor;
}
