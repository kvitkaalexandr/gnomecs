#include "allocators/systemallocator.h"

void *__gPtrToAbsPtr(const gAllocator *allocator, const gPtr ptr) {
    return (void*)ptr;
}

gPtr __absPtrToGPtr(const gAllocator *allocator, const void *ptr) {
    return (gPtr)ptr;
}

gPtr __alloc(gAllocator *allocator, const size_t size) {
    return (gPtr)malloc(size);
}

gPtr __reAlloc(gAllocator *allocator, const gPtr ptr, const size_t newSize) {
    return (gPtr)realloc((void*)ptr, newSize);
}

void __free(gAllocator *allocator, const gPtr ptr) {
    free((void*)ptr);
}

gAllocator *gSystemAllocatorCreate() {
    gAllocator* a = malloc(sizeof(gAllocator));
    if (a == NULL) return NULL;

    a->heap = NULL;

    a->alloc = __alloc;
    a->reAlloc = __reAlloc;
    a->free = __free;
    a->relToAbs = __gPtrToAbsPtr;
    a->absToRel = __absPtrToGPtr;

    return a;
}