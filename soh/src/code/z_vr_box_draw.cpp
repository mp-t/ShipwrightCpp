#include "global.h"

Mtx* sSkyboxDrawMatrix;

Mtx* SkyboxDraw_UpdateMatrix(SkyboxContext* skyboxCtx, f32 x, f32 y, f32 z) {
    Matrix_Translate(x, y, z, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateX(skyboxCtx->rot.x, MTXMODE_APPLY);
    Matrix_RotateY(skyboxCtx->rot.y, MTXMODE_APPLY);
    Matrix_RotateZ(skyboxCtx->rot.z, MTXMODE_APPLY);
    return Matrix_ToMtx(sSkyboxDrawMatrix, "../z_vr_box_draw.c", 42);
}

void SkyboxDraw_Draw(SkyboxContext* skyboxCtx, GraphicsContext* gfxCtx, s16 skyboxId, s16 blend, f32 x, f32 y, f32 z) {
    OPEN_DISPS(gfxCtx, "../z_vr_box_draw.c", 52);

    func_800945A0(gfxCtx);

    //gsSPShaderTest(POLY_OPA_DISP++);
    gSPInvalidateTexCache(POLY_OPA_DISP++, 0);
    
    // OTRTODO: Not working...

    /*for (int i = 0; i < 8; i++) 
    {
        if (skyboxCtx->staticSegments[0] != NULL)
            gSPInvalidateTexCache(POLY_OPA_DISP++, (uintptr_t)skyboxCtx->staticSegments[0] + (0x10000 * i));
        
        if (skyboxCtx->staticSegments[1] != NULL)
            gSPInvalidateTexCache(POLY_OPA_DISP++, (uintptr_t)skyboxCtx->staticSegments[1] + (0x10000 * i));
    }*/

    gSPSegment(POLY_OPA_DISP++, 0x7, skyboxCtx->staticSegments[0]);
    gSPSegment(POLY_OPA_DISP++, 0x8, skyboxCtx->staticSegments[1]);
    gSPSegment(POLY_OPA_DISP++, 0x9, skyboxCtx->palettes);

    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, blend);
    gSPTexture(POLY_OPA_DISP++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);

    sSkyboxDrawMatrix = static_cast<Mtx*>(Graph_Alloc(gfxCtx, sizeof(Mtx)));

    Matrix_Translate(x, y, z, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateX(skyboxCtx->rot.x, MTXMODE_APPLY);
    Matrix_RotateY(skyboxCtx->rot.y, MTXMODE_APPLY);
    Matrix_RotateZ(skyboxCtx->rot.z, MTXMODE_APPLY);
    Matrix_ToMtx(sSkyboxDrawMatrix, "../z_vr_box_draw.c", 76);
    gSPMatrix(POLY_OPA_DISP++, sSkyboxDrawMatrix, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gDPSetColorDither(POLY_OPA_DISP++, G_CD_MAGICSQ);
    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

    gDPLoadTLUT_pal256(POLY_OPA_DISP++, skyboxCtx->palettes[0]);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);
    gDPSetTextureConvert(POLY_OPA_DISP++, G_TC_FILT);

    if (skyboxCtx->unk_140) {
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[0]);
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[1]);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPLoadTLUT_pal256(POLY_OPA_DISP++, skyboxCtx->palettes[1]);
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[2]);
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[3]);

        if (skyboxId != SKYBOX_BAZAAR) {
            if (skyboxId <= SKYBOX_HOUSE_KAKARIKO || skyboxId > SKYBOX_BOMBCHU_SHOP) {
                gDPPipeSync(POLY_OPA_DISP++);
                gDPLoadTLUT_pal256(POLY_OPA_DISP++, skyboxCtx->palettes[2]);
                gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[4]);
                gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[5]);

                gDPPipeSync(POLY_OPA_DISP++);
                if (skyboxCtx->unk_140 != 2) {
                    gDPLoadTLUT_pal256(POLY_OPA_DISP++, skyboxCtx->palettes[3]);
                    gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[6]);
                    gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[7]);
                }
            }
        }
    } else {
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[0]);
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[2]);
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[4]);
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[6]);
        gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[8]);
        if (skyboxId == SKYBOX_CUTSCENE_MAP) {
            gSPDisplayList(POLY_OPA_DISP++, skyboxCtx->dListBuf[10]);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    //gsSPShaderTest2(POLY_OPA_DISP++);


    CLOSE_DISPS(gfxCtx, "../z_vr_box_draw.c", 125);
}

void SkyboxDraw_Update(SkyboxContext* skyboxCtx) {
}
