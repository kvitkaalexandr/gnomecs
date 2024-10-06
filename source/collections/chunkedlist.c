#include "collections/chunkedlist.h"

#include <string.h>

bool gChunkedListInit(const gAllocator *allocator, gChunkedList *chunkedList, const size_t elementSize, const size_t chunkSize) {
    chunkedList->elementSize = elementSize;
    chunkedList->chunkSize = chunkSize;
    chunkedList->count = 0;
    return gVectorInit(allocator, &chunkedList->chunkPointers, sizeof(gSlice), gChunkedListPointerCapacity);
}

void gChunkedListFree(const gAllocator *allocator, gChunkedList *chunkedList) {
    for (size_t i = 0; i < chunkedList->chunkPointers.count; i++) {
        gSlice *slice = gVectorAt(allocator, &chunkedList->chunkPointers, i);
        gSliceFree(allocator, slice);
    }
    gVectorFree(allocator, &chunkedList->chunkPointers);
}

bool gChunkedListAllocChunk(gAllocator *allocator, gChunkedList *chunkedList) {
    const size_t byteSize = chunkedList->elementSize * chunkedList->chunkSize;

    const gPtr newChunk = gAllocatorAlloc(allocator, byteSize);
    if (newChunk == gNull) return false;
    memset(gPtrToAbsPtr(allocator, newChunk), 0, byteSize);

    gSlice slice;
    gSliceInit(&slice, newChunk, chunkedList->elementSize, chunkedList->chunkSize);

    return gVectorAdd(allocator, &chunkedList->chunkPointers, &slice);
}

bool __gChunkedListPushBackDefault(gAllocator *allocator, gChunkedList *chunkedList, const size_t head) {
    if (chunkedList->chunkPointers.count == 0) {
        if (!gChunkedListAllocChunk(allocator, chunkedList))
            return false;
    }

    if (head >= chunkedList->chunkPointers.count) {
        if (!gChunkedListAllocChunk(allocator, chunkedList))
            return false;
    }

    chunkedList->count++;

    return true;
}

bool gChunkedListPushBackDefault(gAllocator *allocator, gChunkedList *chunkedList) {
    const size_t head = chunkedList->count / chunkedList->chunkSize;
    return __gChunkedListPushBackDefault(allocator, chunkedList, head);
}

bool gChunkedListPushBack(gAllocator *allocator, gChunkedList *chunkedList, const void *element) {
    const size_t head = chunkedList->count / chunkedList->chunkSize;

    if (!__gChunkedListPushBackDefault(allocator, chunkedList, head)) return false;

    gSlice *slice = gVectorAt(allocator, &chunkedList->chunkPointers, head);
    if (slice == NULL) return false;

    const size_t i = (chunkedList->count - 1) % chunkedList->chunkSize;
    if (!gSliceSet(allocator, slice, i, element)) return false;

    return true;
}

void *gChunkedListAt(const gAllocator *allocator, const gChunkedList *chunkedList, const size_t index) {
    if (index >= chunkedList->count) return NULL;
    const size_t head = index / chunkedList->chunkSize;
    const size_t i = index % chunkedList->chunkSize;
    gSlice *slice = gVectorAt(allocator, &chunkedList->chunkPointers, head);
    if (slice == NULL) return NULL;
    return gSliceAt(allocator, slice, i);
}


bool gChunkedListSet(const gAllocator *allocator, const gChunkedList *chunkedList, const size_t index, const void *element) {
    if (index >= chunkedList->count) return false;
    const size_t head = index / chunkedList->chunkSize;
    const size_t i = index % chunkedList->chunkSize;
    gSlice *slice = gVectorAt(allocator, &chunkedList->chunkPointers, head);
    if (slice == NULL) return false;
    return gSliceSet(allocator, slice, i, element);
}