#include "global.h"

void GameAlloc_Log(GameAlloc* thisx) {
    GameAllocEntry* iter;

    osSyncPrintf("thisx = %08x\n", thisx);

    iter = thisx->base.next;
    while (iter != &thisx->base) {
        osSyncPrintf("ptr = %08x size = %d\n", iter, iter->size);
        iter = iter->next;
    }
}

void* GameAlloc_MallocDebug(GameAlloc* thisx, size_t size, const char* file, s32 line) {
    GameAllocEntry* ptr = static_cast<GameAllocEntry*>(SystemArena_MallocDebug(size + sizeof(GameAllocEntry), file, line));

    if (ptr != NULL) {
        ptr->size = size;
        ptr->prev = thisx->head;
        thisx->head->next = ptr;
        thisx->head = ptr;
        ptr->next = &thisx->base;
        thisx->base.prev = thisx->head;
        return ptr + 1;
    } else {
        return NULL;
    }
}

void* GameAlloc_Malloc(GameAlloc* thisx, size_t size) {
    GameAllocEntry* ptr = static_cast<GameAllocEntry*>(SystemArena_MallocDebug(size + sizeof(GameAllocEntry), "../gamealloc.c", 93));

    if (ptr != NULL) {
        ptr->size = size;
        ptr->prev = thisx->head;
        thisx->head->next = ptr;
        thisx->head = ptr;
        ptr->next = &thisx->base;
        thisx->base.prev = thisx->head;
        return ptr + 1;
    } else {
        return NULL;
    }
}

void GameAlloc_Free(GameAlloc* thisx, void* data) {
    GameAllocEntry* ptr;

    if (data != NULL) {
        ptr = &((GameAllocEntry*)data)[-1];
        LogUtils_CheckNullPointer("ptr->prev", ptr->prev, "../gamealloc.c", 120);
        LogUtils_CheckNullPointer("ptr->next", ptr->next, "../gamealloc.c", 121);
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        thisx->head = thisx->base.prev;
        SystemArena_FreeDebug(ptr, "../gamealloc.c", 125);
    }
}

void GameAlloc_Cleanup(GameAlloc* thisx) {
    GameAllocEntry* next = thisx->base.next;
    GameAllocEntry* cur;

    while (&thisx->base != next) {
        cur = next;
        next = next->next;
        SystemArena_FreeDebug(cur, "../gamealloc.c", 145);
    }

    thisx->head = &thisx->base;
    thisx->base.next = &thisx->base;
    thisx->base.prev = &thisx->base;
}

void GameAlloc_Init(GameAlloc* thisx) {
    thisx->head = &thisx->base;
    thisx->base.next = &thisx->base;
    thisx->base.prev = &thisx->base;
}
