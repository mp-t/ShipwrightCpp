#include "global.h"

ListAlloc* ListAlloc_Init(ListAlloc* thisx) {
    thisx->prev = NULL;
    thisx->next = NULL;
    return thisx;
}

void* ListAlloc_Alloc(ListAlloc* thisx, size_t size) {
    ListAlloc* ptr = static_cast<ListAlloc*>(SystemArena_MallocDebug(size + sizeof(ListAlloc), "../listalloc.c", 40));
    ListAlloc* next;

    if (ptr == NULL) {
        return NULL;
    }

    next = thisx->next;
    if (next != NULL) {
        next->next = ptr;
    }

    ptr->prev = next;
    ptr->next = NULL;
    thisx->next = ptr;

    if (thisx->prev == NULL) {
        thisx->prev = ptr;
    }

    return (u8*)ptr + sizeof(ListAlloc);
}

void ListAlloc_Free(ListAlloc* thisx, void* data) {
    ListAlloc* ptr = &((ListAlloc*)data)[-1];

    if (ptr->prev != NULL) {
        ptr->prev->next = ptr->next;
    }

    if (ptr->next != NULL) {
        ptr->next->prev = ptr->prev;
    }

    if (thisx->prev == ptr) {
        thisx->prev = ptr->next;
    }

    if (thisx->next == ptr) {
        thisx->next = ptr->prev;
    }

    SystemArena_FreeDebug(ptr, "../listalloc.c", 72);
}

void ListAlloc_FreeAll(ListAlloc* thisx) {
    ListAlloc* iter = thisx->prev;

    while (iter != NULL) {
        ListAlloc_Free(thisx, (u8*)iter + sizeof(ListAlloc));
        iter = thisx->prev;
    }
}
