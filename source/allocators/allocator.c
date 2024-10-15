#include "allocators/allocator.h"
#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>

void *gPtrToAbsPtr(const gAllocator *allocator, const gPtr ptr) {
    return (void*)allocator->heap + ptr;
}

gPtr absPtrToGPtr(const gAllocator *allocator, const void *ptr) {
    return (gPtr)ptr - (gPtr)allocator->heap;
}

gAllocator* gAllocatorCreate(const size_t heapSize, const size_t defaultBlockSize) {
    gAllocator* a = malloc(sizeof(gAllocator));
    if (a == NULL) return NULL;

    a->heap = malloc(heapSize);
    if (a->heap == NULL) {
        free(a);
        return NULL;
    }

    gAllocatorState* state = gAllocatorGetState(a);
    state->size = heapSize;
    state->freeBlock = gNull;
    state->head = sizeof(gAllocatorState);
    state->defaultBlockSize = defaultBlockSize;

    a->alloc = gAllocatorAlloc;
    a->reAlloc = gAllocatorReAlloc;
    a->free = gAllocatorFree;
    a->relToAbs = gPtrToAbsPtr;
    a->absToRel = absPtrToGPtr;

    return a;
}

void gAllocatorSelfFree(gAllocator* allocator) {
    if (allocator == NULL) return;
    if (allocator->heap != NULL) free(allocator->heap);
    free(allocator);
}

void gAllocatorInitBlock(gAllocatorBlock* block, size_t size) {
    block->size = size;
    block->status = gAllocatorBlockFree;
    block->nextFree = gNull;
}

void gAllocatorUseBlock(gAllocatorBlock* block) {
    assert(block != NULL);
    assert(block->status == gAllocatorBlockFree);
    block->status = gAllocatorBlockUsed;
    block->nextFree = gNull;
}

gPtr gAllocatorAlloc(const gAllocator* allocator, const size_t size) {
    assert(size > 0);
    gAllocatorState* state = gAllocatorGetState(allocator);
    const size_t fittedSize = max(size, state->defaultBlockSize);
    const size_t alignedSize = align(fittedSize, alignof(max_align_t));

    gPtr freeBlock = state->freeBlock;
    while (freeBlock != gNull) {
        gAllocatorBlock* block = allocator->relToAbs(allocator, freeBlock);
        if (block->size >= alignedSize) {
            gAllocatorUseBlock(block);
            state->freeBlock = block->nextFree;
            return freeBlock + sizeof(gAllocatorBlock);
        }
        freeBlock = block->nextFree;
    }

    if (freeBlock == gNull) {
        const gPtr newHead = state->head + sizeof(gAllocatorBlock) + alignedSize;

        //todo: Different strategy for when the heap is full
        //1. Fixed size heap (current)
        //2. Extendable single-block heap (allocate new heap and copy old heap)
        //3. Extendable multi-block heap (allocate new heap and link to old heap)
        if (newHead >= state->size) return gNull;

        gAllocatorBlock* newBlock = allocator->relToAbs(allocator, state->head);
        gAllocatorInitBlock(newBlock, alignedSize);
        gAllocatorUseBlock(newBlock);

        const gPtr userMemory = state->head + sizeof(gAllocatorBlock);
        state->head = newHead;

        return userMemory;
    }

    return gNull;
}

void gAllocatorFree(const gAllocator* allocator, const gPtr ptr) {
    assert(ptr != gNull);
    gAllocatorState* state = gAllocatorGetState(allocator);
    const gPtr blockPtr = ptr - sizeof(gAllocatorBlock);
    gAllocatorBlock* block = allocator->relToAbs(allocator, blockPtr);
    block->status = gAllocatorBlockFree;
    block->nextFree = state->freeBlock;
    state->freeBlock = blockPtr;
}

gPtr gAllocatorReAlloc(const gAllocator *allocator, const gPtr ptr, const size_t newSize) {
    assert(ptr != gNull);
    const gAllocatorBlock* block = gAllocatorGetBlock(allocator, ptr);
    if (block->size >= newSize) return ptr;

    const gPtr newPtr = gAllocatorAlloc(allocator, newSize);
    if (newPtr == gNull) return gNull;

    memcpy(allocator->relToAbs(allocator, newPtr), allocator->relToAbs(allocator, ptr), newSize);
    gAllocatorFree(allocator, ptr);
    return newPtr;
}

gAllocatorBlock* gAllocatorGetBlock(const gAllocator* allocator, const gPtr ptr) {
    const gPtr blockPtr = ptr - sizeof(gAllocatorBlock);
    gAllocatorBlock* block = allocator->relToAbs(allocator, blockPtr);
    return block;
}

void gAllocatorCopyTo(const gAllocator *allocator, void *dest) {
    gAllocatorState *state = gAllocatorGetState(allocator);
    memcpy(dest, allocator->heap, state->head);
}

void gAllocatorCopyFrom(gAllocator *allocator, const void *src, const size_t size) {
    memcpy(allocator->heap, src, size);
}