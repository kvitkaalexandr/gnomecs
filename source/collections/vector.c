#include "collections/vector.h"
#include <string.h>

bool gVectorInit(gAllocator *allocator, gVector *vector, const size_t elementSize, const size_t initialCapacity) {
    vector->elementSize = elementSize;
    vector->capacity = initialCapacity;
    vector->count = 0;

    const size_t byteSize = initialCapacity * elementSize;
    const gPtr ptr = allocator->alloc(allocator, byteSize);
    if (ptr == gNull) return false;
    memset(allocator->relToAbs(allocator, ptr), 0, byteSize);

    gSliceInit(&vector->data, ptr, elementSize, initialCapacity);
    return true;
}

bool gVectorAdd(gAllocator *allocator, gVector *vector, const void *element) {
    if (vector->count == vector->capacity) {
        vector->capacity *= 2;
        const gPtr newData = allocator->reAlloc(allocator, vector->data.data, vector->capacity * vector->elementSize);
        if (newData == gNull) return false;
        gSliceInit(&vector->data, newData, vector->elementSize, vector->capacity);
    }

    void *newItemPtr = gSliceAt(allocator, &vector->data, vector->count);
    if (newItemPtr == NULL) return false;
    memcpy(newItemPtr, element, vector->elementSize);

    vector->count++;

    return true;
}

bool gVectorRemove(const gAllocator *allocator, gVector *vector, const size_t index) {
    if (index >= vector->count) return false;

    if (index == vector->count - 1) {
        vector->count--;
        return true;
    }

    void *itemPtr = gSliceAt(allocator, &vector->data, index);
    const void *nextItemPtr = gSliceAt(allocator, &vector->data, index + 1);
    size_t bytesToMove = (vector->count - index - 1) * vector->elementSize;
    memcpy(itemPtr, nextItemPtr, bytesToMove);

    vector->count--;

    return true;
}

bool gVectorSet(const gAllocator *allocator, gVector *vector, const size_t index, const void *element) {
    if (index >= vector->count) return false;
    void *itemPtr = gSliceAt(allocator, &vector->data, index);
    memcpy(itemPtr, element, vector->elementSize);
    return true;
}

void *gVectorAt(const gAllocator *allocator, const gVector *vector, const size_t index) {
    return gSliceAt(allocator, &vector->data, index);
}

void gVectorClear(gVector *vector) {
    vector->count = 0;
    vector->data.data = gNull;
    vector->data.count = 0;
}

void gVectorFree(gAllocator *allocator, gVector *vector) {
    if (vector->data.data == gNull) return;
    allocator->free(allocator, vector->data.data);
    vector->data.data = gNull;
}