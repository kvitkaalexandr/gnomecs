#include "collections/slice.h"
#include <string.h>

void gSliceInit(gSlice *slice, const gPtr ptr, const size_t size, const size_t count) {
    slice->size = size;
    slice->count = count;
    slice->data = ptr;
}

void *gSliceAt(const gAllocator *allocator, const gSlice *slice, const size_t index) {
    if (index >= slice->count) return NULL;
    return gPtrToAbsPtr(allocator, slice->data + index * slice->size);
}

bool gSliceSet(const gAllocator *allocator, gSlice *slice, const size_t index, const void *element) {
    if (index >= slice->count) return false;
    void *itemPtr = gPtrToAbsPtr(allocator, slice->data + index * slice->size);
    memcpy(itemPtr, element, slice->size);
    return true;
}

void gSliceFree(const gAllocator *allocator, gSlice *slice) {
    if (slice->data != gNull) gAllocatorFree(allocator, slice->data);
    slice->data = gNull;
    slice->count = 0;
    slice->size = 0;
}