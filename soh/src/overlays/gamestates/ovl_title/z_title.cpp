/*
 * File: z_title.c
 * Overlay: ovl_title
 * Description: Displays the Nintendo Logo
 */

#define NORMAL_GAMEPLAY

#include "global.h"
#include "alloca.h"
#include "textures/nintendo_rogo_static/nintendo_rogo_static.h"
#include <soh/Enhancements/bootcommands.h>

const char* quote;

void Title_PrintBuildInfo(Gfx** gfxp) {
    Gfx* g;
    //GfxPrint* printer;
    GfxPrint printer;

    g = *gfxp;
    g = func_8009411C(g);
    //printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, g);
    GfxPrint_SetColor(&printer, 255, 155, 255, 255);
    GfxPrint_SetPos(&printer, 12, 20);

#ifdef _MSC_VER
    GfxPrint_Printf(&printer, "MSVC SHIP");
#else
    GfxPrint_Printf(printer, "GCC SHIP");
#endif

    GfxPrint_SetColor(&printer, 255, 255, 255, 255);
    GfxPrint_SetPos(&printer, 2, 22);
    GfxPrint_Printf(&printer, quote);
    GfxPrint_SetPos(&printer, 1, 25);
    GfxPrint_Printf(&printer, "Build Date:%s", gBuildDate);
    GfxPrint_SetPos(&printer, 3, 26);
    GfxPrint_Printf(&printer, "%s", gBuildTeam);
    GfxPrint_SetPos(&printer, 3, 28);
    GfxPrint_Printf(&printer, "Release Version: %s", gBuildVersion);
    g = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
    *gfxp = g;
}

const char* quotes[11] = {
    "My boy! This peace is what all true warriors strive for!",
    "Hmm. How can we help?",
    "Zelda! Duke Onkled is under attack by the evil forces of Ganon!",
    "I'm going to Gamelon to aid him.",
    "I'll take the Triforce of Courage to protect me.",
    "If you don't hear from me in a month, send Link.",
    "Enough! My ship sails in the morning.",
    "I wonder what's for dinner.",
    "You've saved me!",
    "After you've scrubbed all the floors in Hyrule, then we can talk about mercy! Take him away!",
    "Waaaahahahohohahahahahahaha"
};

const char* SetQuote() {
    srand(time(NULL));
    int randomQuote = rand() % 11;
    return quotes[randomQuote];
}

// Note: In other rom versions thisv function also updates unk_1D4, coverAlpha, addAlpha, visibleDuration to calculate
// the fade-in/fade-out + the duration of the n64 logo animation
void Title_Calc(TitleContext* thisv) {
#ifdef NORMAL_GAMEPLAY
    if ((thisv->coverAlpha == 0) && (thisv->visibleDuration != 0)) {
        thisv->visibleDuration--;
        thisv->unk_1D4--;
        if (thisv->unk_1D4 == 0) {
            thisv->unk_1D4 = 400;
        }
    } else {
        thisv->coverAlpha += thisv->addAlpha;
        if (thisv->coverAlpha <= 0) {
            thisv->coverAlpha = 0;
            thisv->addAlpha = 3;
        } else if (thisv->coverAlpha >= 0xFF) {
            thisv->coverAlpha = 0xFF;
            thisv->exit = 1;
        }
    }
    thisv->uls = thisv->ult & 0x7F;
    thisv->ult++;

    if (gSkipLogoTest || gLoadFileSelect) {
        thisv->exit = 1;
    }

#else
    thisv->exit = 1;
#endif
}

void Title_SetupView(TitleContext* thisv, f32 x, f32 y, f32 z) {
    View* view = &thisv->view;
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = x;
    eye.y = y;
    eye.z = z;
    up.x = up.z = 0.0f;
    lookAt.x = lookAt.y = lookAt.z = 0.0f;
    up.y = 1.0f;

    func_800AA460(view, 30.0f, 10.0f, 12800.0f);
    func_800AA358(view, &eye, &lookAt, &up);
    func_800AAA50(view, 0xF);
}

void Title_Draw(TitleContext* thisv) {
    static s16 sTitleRotY = 0;
    static Lights1 sTitleLights = gdSPDefLights1(0x64, 0x64, 0x64, 0xFF, 0xFF, 0xFF, 0x45, 0x45, 0x45);

    u16 y;
    u16 idx;
    s32 pad1;
    Vec3f v3;
    Vec3f v1;
    Vec3f v2;
    s32 pad2[2];

    OPEN_DISPS(thisv->state.gfxCtx, "../z_title.c", 395);

    v3.x = 69;
    v3.y = 69;
    v3.z = 69;
    v2.x = -4949.148;
    v2.y = 4002.5417;
    v1.x = 0;
    v1.y = 0;
    v1.z = 0;
    v2.z = 1119.0837;

    char* n64LogoTex = ResourceMgr_LoadTexByName(nintendo_rogo_static_Tex_000000);

    func_8002EABC(&v1, &v2, &v3, thisv->state.gfxCtx);
    gSPSetLights1(POLY_OPA_DISP++, sTitleLights);
    Title_SetupView(thisv, 0, 150.0, 300.0);
    func_80093D18(thisv->state.gfxCtx);
    Matrix_Translate(-53.0, -5.0, 0, MTXMODE_NEW);
    Matrix_Scale(1.0, 1.0, 1.0, MTXMODE_APPLY);
    Matrix_RotateZYX(0, sTitleRotY, 0, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_title.c", 424), G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, gNintendo64LogoDL);
    func_800944C4(thisv->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF2, G_RM_OPA_CI | CVG_DST_WRAP);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
        COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 255, 128);

    gDPLoadMultiBlock(POLY_OPA_DISP++, ResourceMgr_LoadTexByName(nintendo_rogo_static_Tex_001800), 0x100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 11);

    for (idx = 0, y = 94; idx < 16; idx++, y += 2)
    {
        gDPLoadTextureBlock(POLY_OPA_DISP++, &n64LogoTex[0x180 * idx], G_IM_FMT_I,
            G_IM_SIZ_8b, 192, 2, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetTileSize(POLY_OPA_DISP++, 1, thisv->uls, (thisv->ult & 0x7F) - idx * 4, 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 388, y << 2, 1156, (y + 2) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    Environment_FillScreen(thisv->state.gfxCtx, 0, 0, 0, (s16)thisv->coverAlpha, FILL_SCREEN_XLU);

    sTitleRotY += 300;

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_title.c", 483);
}

void Title_Main(GameState* thisx) {
    TitleContext* thisv = (TitleContext*)thisx;

    OPEN_DISPS(thisv->state.gfxCtx, "../z_title.c", 494);

    gSPSegment(POLY_OPA_DISP++, 0, NULL);
    gSPSegment(POLY_OPA_DISP++, 1, thisv->staticSegment);
    func_80095248(thisv->state.gfxCtx, 0, 0, 0);
    Title_Calc(thisv);
    Title_Draw(thisv);

    if (1) {
        Gfx* gfx = POLY_OPA_DISP;
        s32 pad;

        Title_PrintBuildInfo(&gfx);
        POLY_OPA_DISP = gfx;
    }

    if (thisv->exit) {
        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = 1;
        thisv->state.running = false;

        if (gLoadFileSelect)
            SET_NEXT_GAMESTATE(&thisv->state, FileChoose_Init, FileChooseContext);
        else
            SET_NEXT_GAMESTATE(&thisv->state, Opening_Init, OpeningContext);
    }

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_title.c", 541);
}

void Title_Destroy(GameState* thisx) {
    TitleContext* thisv = (TitleContext*)thisx;

    Sram_InitSram(&thisv->state, &thisv->sramCtx);
}

void Title_Init(GameState* thisx) {
    //u32 size = 0;
    TitleContext* thisv = (TitleContext*)thisx;

    quote = SetQuote();

    //thisv->staticSegment = GameState_Alloc(&thisv->state, size, "../z_title.c", 611);
    osSyncPrintf("z_title.c\n");
    //ASSERT(thisv->staticSegment != NULL, "thisv->staticSegment != NULL", "../z_title.c", 614);

    //ResourceMgr_CacheDirectory("nintendo_rogo_static*");

    R_UPDATE_RATE = 1;
    Matrix_Init(&thisv->state);
    View_Init(&thisv->view, thisv->state.gfxCtx);
    thisv->state.main = Title_Main;
    thisv->state.destroy = Title_Destroy;
    thisv->exit = false;
    gSaveContext.fileNum = 0xFF;
    Sram_Alloc(&thisv->state, &thisv->sramCtx);
    thisv->ult = 0;
    thisv->unk_1D4 = 0x14;
    thisv->coverAlpha = 255;
    thisv->addAlpha = -3;
    thisv->visibleDuration = 0x3C;
}
