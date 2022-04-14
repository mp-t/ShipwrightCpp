#include "global.h"

//uintptr_t D_8012DBA0 = (uintptr_t)&D_80155F50;
//uintptr_t D_8012DBA4 = (uintptr_t)&D_80157580;

u64* SysUcode_GetUCodeBoot() {
    //return &D_80009320;
    return nullptr;
}

u32 SysUcode_GetUCodeBootSize() {
    //return (uintptr_t)&D_800093F0 - (uintptr_t)&D_80009320;
    return 0;
}

u64* SysUcode_GetUCode() {
    //return D_8012DBA0;
    return nullptr;
}

u64* SysUcode_GetUCodeData() {
    //return D_8012DBA4;
    return nullptr;
}
