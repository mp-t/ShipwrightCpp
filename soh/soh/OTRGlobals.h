#pragma once

#include "GlobalCtx2.h"
#include "ultra64/gbi.h"

#include <cstdint>

class OTRGlobals
{
public:
	static OTRGlobals* Instance;

	std::shared_ptr<Ship::GlobalCtx2> context;

	OTRGlobals();
	~OTRGlobals();

private:

};

struct CollisionHeader;
struct SkeletonHeader;
struct Drum;
struct Instrument;
struct SoundFontSound;
struct SoundFontSample;
struct AdpcmLoop;
struct AdpcmBook;
struct GfxPrint;
struct AnimationHeaderCommon;
struct GlobalContext;
struct RoomContext;

void InitOTR();
void Graph_ProcessFrame(void (*run_one_game_iter)());
void Graph_ProcessGfxCommands(Gfx* commands);
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, GfxPrint* printer, void (*printImpl)(GfxPrint*, u8));
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
CollisionHeader* ResourceMgr_LoadColByName(const char* path);
uint64_t GetPerfCounter();
SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path);
int ResourceMgr_OTRSigCheck(const void* imgData);
uint64_t osGetTime(void);
uint32_t osGetCount(void);
uint32_t OTRGetCurrentWidth(void);
uint32_t OTRGetCurrentHeight(void);
float OTRGetAspectRatio(void);
float OTRGetDimensionFromLeftEdge(float v);
float OTRGetDimensionFromRightEdge(float v);
int16_t OTRGetRectDimensionFromLeftEdge(float v);
int16_t OTRGetRectDimensionFromRightEdge(float v);
void bswapDrum(Drum* swappable);
void bswapInstrument(Instrument* swappable);
void bswapSoundFontSound(SoundFontSound* swappable);
void bswapSoundFontSample(SoundFontSample* swappable);
void bswapAdpcmLoop(AdpcmLoop* swappable);
void bswapAdpcmBook(AdpcmBook* swappable);
char* ResourceMgr_LoadFileRaw(const char* resName);
bool AudioPlayer_Init(void);
int AudioPlayer_Buffered(void);
int AudioPlayer_GetDesiredBuffered(void);
void AudioPlayer_Play(const uint8_t* buf, uint32_t len);
void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);

u8* ResourceMgr_LoadCSByName(const char* path);
AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path);
void OTRGameplay_SpawnScene(GlobalContext* globalCtx, s32 sceneNum, s32 spawn);
void OTRMessage_Init();
char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path);
char* ResourceMgr_LoadJPEG(const char* data, int dataSize);
s32 OTRfunc_8009728C(GlobalContext* globalCtx, RoomContext* roomCtx, s32 roomNum);
s32 OTRfunc_800973FC(GlobalContext* globalCtx, RoomContext* roomCtx);
void ResourceMgr_WriteTexS16ByName(const char* texPath, int index, s16 value);
void ResourceMgr_DirtyDirectory(const char* resName);