#include "global.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"

static Vtx sVertices[5] = {
    VTX(-32, -32, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 32, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -32, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
};

// original name: "EffectShieldParticle_ct"
void EffectShieldParticle_Init(void* thisv, void* initParamsx) {
    EffectShieldParticle* thisx = (EffectShieldParticle*)thisv;
    EffectShieldParticleInit* initParams = (EffectShieldParticleInit*)initParamsx;
    EffectShieldParticleElement* elem;

    if ((thisx != NULL) && (initParams != NULL)) {
        thisx->numElements = initParams->numElements;
        if (thisx->numElements > ARRAY_COUNT(thisx->elements)) {
            osSyncPrintf(VT_FGCOL(RED));
            osSyncPrintf("EffectShieldParticle_ct():パーティクル数がオーバしてます。\n");
            osSyncPrintf(VT_RST);
            return;
        }

        thisx->position = initParams->position;
        thisx->primColorStart = initParams->primColorStart;
        thisx->envColorStart = initParams->envColorStart;
        thisx->primColorMid = initParams->primColorMid;
        thisx->envColorMid = initParams->envColorMid;
        thisx->primColorEnd = initParams->primColorEnd;
        thisx->envColorEnd = initParams->envColorEnd;
        thisx->deceleration = initParams->deceleration;
        thisx->maxInitialSpeed = initParams->maxInitialSpeed;
        thisx->lengthCutoff = initParams->lengthCutoff;
        thisx->duration = initParams->duration;
        thisx->timer = 0;

        for (elem = &thisx->elements[0]; elem < &thisx->elements[thisx->numElements]; elem++) {
            elem->initialSpeed = (Rand_ZeroOne() * (thisx->maxInitialSpeed * 0.5f)) + (thisx->maxInitialSpeed * 0.5f);
            elem->endX = 0.0f;
            elem->startXChange = 0.0f;
            elem->startX = 0.0f;
            elem->endXChange = elem->initialSpeed;
            elem->yaw = Rand_ZeroOne() * 65534.0f;
            elem->pitch = Rand_ZeroOne() * 65534.0f;
        }

        thisx->lightDecay = initParams->lightDecay;
        if (thisx->lightDecay == true) {
            thisx->lightInfo.type = LIGHT_POINT_NOGLOW;
            thisx->lightInfo.params.point = initParams->lightPoint;
            thisx->lightNode =
                LightContext_InsertLight(Effect_GetGlobalCtx(), &Effect_GetGlobalCtx()->lightCtx, &thisx->lightInfo);
        } else {
            thisx->lightNode = NULL;
        }
    }
}

void EffectShieldParticle_Destroy(void* thisv) {
    EffectShieldParticle* thisx = (EffectShieldParticle*)thisv;

    if ((thisx != NULL) && (thisx->lightDecay == true)) {
        if (thisx->lightNode == Effect_GetGlobalCtx()->lightCtx.listHead) {
            Effect_GetGlobalCtx()->lightCtx.listHead = thisx->lightNode->next;
        }
        LightContext_RemoveLight(Effect_GetGlobalCtx(), &Effect_GetGlobalCtx()->lightCtx, thisx->lightNode);
    }
}

s32 EffectShieldParticle_Update(void* thisv) {
    EffectShieldParticle* thisx = (EffectShieldParticle*)thisv;
    EffectShieldParticleElement* elem;

    if (thisx == NULL) {
        return 0;
    }

    for (elem = &thisx->elements[0]; elem < &thisx->elements[thisx->numElements]; elem++) {
        elem->endXChange -= thisx->deceleration;
        if (elem->endXChange < 0.0f) {
            elem->endXChange = 0.0f;
        }

        if (elem->startXChange > 0.0f) {
            elem->startXChange -= thisx->deceleration;
            if (elem->startXChange < 0.0f) {
                elem->startXChange = 0.0f;
            }
        }

        elem->endX += elem->endXChange;
        elem->startX += elem->startXChange;

        if ((elem->startXChange == 0.0f) && (thisx->lengthCutoff < (elem->endX - elem->startX))) {
            elem->startXChange = elem->initialSpeed;
        }
    }

    if (thisx->lightDecay == true) {
        thisx->lightInfo.params.point.radius /= 2;
    }

    thisx->timer++;

    if (thisx->duration < thisx->timer) {
        return 1;
    }

    return 0;
}

void EffectShieldParticle_GetColors(EffectShieldParticle* thisx, Color_RGBA8* primColor, Color_RGBA8* envColor) {
    s32 halfDuration = thisx->duration * 0.5f;
    f32 ratio;

    if (halfDuration == 0) {
        primColor->r = thisx->primColorStart.r;
        primColor->g = thisx->primColorStart.g;
        primColor->b = thisx->primColorStart.b;
        primColor->a = thisx->primColorStart.a;
        envColor->r = thisx->envColorStart.r;
        envColor->g = thisx->envColorStart.g;
        envColor->b = thisx->envColorStart.b;
        envColor->a = thisx->envColorStart.a;
    } else if (thisx->timer < halfDuration) {
        ratio = thisx->timer / (f32)halfDuration;
        primColor->r = thisx->primColorStart.r + (thisx->primColorMid.r - thisx->primColorStart.r) * ratio;
        primColor->g = thisx->primColorStart.g + (thisx->primColorMid.g - thisx->primColorStart.g) * ratio;
        primColor->b = thisx->primColorStart.b + (thisx->primColorMid.b - thisx->primColorStart.b) * ratio;
        primColor->a = thisx->primColorStart.a + (thisx->primColorMid.a - thisx->primColorStart.a) * ratio;
        envColor->r = thisx->envColorStart.r + (thisx->envColorMid.r - thisx->envColorStart.r) * ratio;
        envColor->g = thisx->envColorStart.g + (thisx->envColorMid.g - thisx->envColorStart.g) * ratio;
        envColor->b = thisx->envColorStart.b + (thisx->envColorMid.b - thisx->envColorStart.b) * ratio;
        envColor->a = thisx->envColorStart.a + (thisx->envColorMid.a - thisx->envColorStart.a) * ratio;
    } else {
        ratio = (thisx->timer - halfDuration) / (f32)halfDuration;
        primColor->r = thisx->primColorMid.r + (thisx->primColorEnd.r - thisx->primColorMid.r) * ratio;
        primColor->g = thisx->primColorMid.g + (thisx->primColorEnd.g - thisx->primColorMid.g) * ratio;
        primColor->b = thisx->primColorMid.b + (thisx->primColorEnd.b - thisx->primColorMid.b) * ratio;
        primColor->a = thisx->primColorMid.a + (thisx->primColorEnd.a - thisx->primColorMid.a) * ratio;
        envColor->r = thisx->envColorMid.r + (thisx->envColorEnd.r - thisx->envColorMid.r) * ratio;
        envColor->g = thisx->envColorMid.g + (thisx->envColorEnd.g - thisx->envColorMid.g) * ratio;
        envColor->b = thisx->envColorMid.b + (thisx->envColorEnd.b - thisx->envColorMid.b) * ratio;
        envColor->a = thisx->envColorMid.a + (thisx->envColorEnd.a - thisx->envColorMid.a) * ratio;
    }
}

void EffectShieldParticle_Draw(void* thisv, GraphicsContext* gfxCtx) {
    EffectShieldParticle* thisx = (EffectShieldParticle*)thisv;
    EffectShieldParticleElement* elem;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;

    OPEN_DISPS(gfxCtx, "../z_eff_shield_particle.c", 272);

    if (thisx != NULL) {
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x26);

        gDPSetCycleType(POLY_XLU_DISP++, G_CYC_2CYCLE);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPTexture(POLY_XLU_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);

        gDPLoadTextureBlock(POLY_XLU_DISP++, gUnknownCircle6Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, 0, 0,
                          0, COMBINED, 0, 0, 0, COMBINED);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
        gSPSetGeometryMode(POLY_XLU_DISP++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);

        EffectShieldParticle_GetColors(thisx, &primColor, &envColor);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, primColor.r, primColor.g, primColor.b, primColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, envColor.r, envColor.g, envColor.b, envColor.a);
        gDPPipeSync(POLY_XLU_DISP++);

        for (elem = &thisx->elements[0]; elem < &thisx->elements[thisx->numElements]; elem++) {
            Mtx* mtx;
            MtxF sp104;
            MtxF spC4;
            MtxF sp84;
            f32 temp1 = (s16)((elem->endX + elem->startX) * 0.5f);
            f32 temp2 = elem->endX - elem->startX;
            f32 temp3 = (s16)((temp2 * (1.0f / 64.0f)) / 0.02f);

            if (temp3 < 1.0f) {
                temp3 = 1.0f;
            }

            SkinMatrix_SetTranslate(&spC4, thisx->position.x, thisx->position.y, thisx->position.z);
            SkinMatrix_SetRotateZYX(&sp104, 0, elem->yaw, 0);
            SkinMatrix_MtxFMtxFMult(&spC4, &sp104, &sp84);
            SkinMatrix_SetRotateZYX(&sp104, 0, 0, elem->pitch);
            SkinMatrix_MtxFMtxFMult(&sp84, &sp104, &spC4);
            SkinMatrix_SetTranslate(&sp104, temp1, 0.0f, 0.0f);
            SkinMatrix_MtxFMtxFMult(&spC4, &sp104, &sp84);
            SkinMatrix_SetScale(&sp104, temp3 * 0.02f, 0.02f, 0.02f);
            SkinMatrix_MtxFMtxFMult(&sp84, &sp104, &spC4);

            mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &spC4);
            if (mtx == NULL) {
                break;
            }

            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPVertex(POLY_XLU_DISP++, reinterpret_cast<std::uintptr_t>(&sVertices[0]), 4, 0);
            gSP2Triangles(POLY_XLU_DISP++, 0, 1, 2, 0, 0, 3, 1, 0);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_shield_particle.c", 359);
}
