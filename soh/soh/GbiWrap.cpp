#include "z64.h"

//OTRTODO - this is awful

void InitOTR();
void Graph_ProcessFrame(void (*run_one_game_iter)(void));
void Graph_ProcessGfxCommands(Gfx* commands);
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char));
void OTRSetFrameDivisor(int divisor);
uint16_t OTRGetPixelDepth(float x, float y);
int32_t OTRGetLastScancode();
void ResourceMgr_CacheDirectory(const char* resName);
void ResourceMgr_LoadFile(const char* resName);
char* ResourceMgr_LoadFileFromDisk(const char* filePath);
char* ResourceMgr_LoadTexByName(const char* texPath);
char* ResourceMgr_LoadTexOrDListByName(char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(char* animPath);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);
Vtx* ResourceMgr_LoadVtxByName(const char* path);
CollisionHeader* ResourceMgr_LoadColByName(char* path);
uint64_t GetPerfCounter();
int ResourceMgr_OTRSigCheck(const void* imgData);

void gSPSegment(void* value, int segNum, const void* target) {
    char* imgData = (char*)target;

    int res = ResourceMgr_OTRSigCheck(imgData);

    if (res)
        target = ResourceMgr_LoadTexOrDListByName(imgData);

    __gSPSegment(value, segNum, target);
}

void gDPSetTextureImage(Gfx* pkt, u32 format, u32 size, u32 width, const void* i) {
    __gDPSetTextureImage(pkt, format, size, width, i);
}

void gDPSetTextureImageFB(Gfx* pkt, u32 format, u32 size, u32 width, int fb)
{
    __gDPSetTextureImageFB(pkt, format, size, width, fb);
}

void gSPDisplayList(Gfx* pkt, const Gfx* dl) {
    const char* imgData = reinterpret_cast<const char*>(dl);

    if (ResourceMgr_OTRSigCheck(imgData) == 1)
        dl = ResourceMgr_LoadGfxByName(imgData);

    __gSPDisplayList(pkt, dl);
}

void gSPDisplayListOffset(Gfx* pkt, const Gfx* dl, int offset) {
    auto imgData = reinterpret_cast<const char*>(dl);

    if (ResourceMgr_OTRSigCheck(imgData) == 1)
        dl = ResourceMgr_LoadGfxByName(imgData);

    __gSPDisplayList(pkt, dl + offset);
}

void gSPVertex(Gfx* pkt, uintptr_t v, int n, int v0) {

    if (ResourceMgr_OTRSigCheck((char*)v) == 1)
        v = (uintptr_t)ResourceMgr_LoadVtxByName((char*)v);

    __gSPVertex(pkt, v, n, v0);
}

void gSPVertex(Gfx* pkt, const char* v, const int n, const int v0)
{
    gSPVertex(pkt, reinterpret_cast<std::uintptr_t>(v), n, v0);
}

void gSPVertex(Gfx* pkt, Vtx* v, const int n, const int v0)
{
    gSPVertex(pkt, reinterpret_cast<std::uintptr_t>(v), n, v0);
}

void gSPInvalidateTexCache(Gfx* pkt, uintptr_t texAddr)
{
    char* imgData = (char*)texAddr;

    if (texAddr != 0 && ResourceMgr_OTRSigCheck(imgData))
        texAddr = (uintptr_t)ResourceMgr_LoadTexByName(imgData);

    __gSPInvalidateTexCache(pkt, texAddr);
}