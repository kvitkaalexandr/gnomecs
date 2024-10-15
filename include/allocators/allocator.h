#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>

/**
 * This allocator allow allocating memory in a fixed-size array
 */

/**
 * Relative pointer used in the allocator
 * Relative pointer contains the offset from the heap start, it allows copying the heap to a different location
 */
typedef uintptr_t gPtr;
#define gNull 0

typedef struct gAllocator{
    void* heap;

    gPtr (*alloc)(const struct gAllocator *allocator, const size_t size);
    gPtr (*reAlloc)(const struct gAllocator *allocator, const gPtr ptr, const size_t newSize);
    void (*free)(const struct gAllocator *allocator,const gPtr ptr);
    void* (*relToAbs)(const struct gAllocator *allocator, const gPtr ptr);
    gPtr (*absToRel)(const struct gAllocator *allocator, const void* ptr);
} gAllocator;

typedef struct {
    size_t size;
    size_t defaultBlockSize;
    gPtr freeBlock;
    gPtr head;
} gAllocatorState;

/**
 * Convert a relative pointer to an absolute pointer
 */
void *gPtrToAbsPtr(const gAllocator *allocator, const gPtr ptr);


/**
 * Convert an absolute pointer to a relative pointer
 */
gPtr absPtrToGPtr(const gAllocator *allocator, const void *ptr);

/**
 * Get the allocator state
 * @param allocator The allocator
 * @return The allocator state
 */
#define gAllocatorGetState(allocator) ((gAllocatorState*)allocator->heap)

#define gAllocatorBlockFree 0
#define gAllocatorBlockUsed 1
typedef struct {
    size_t size;
    gPtr nextFree;
    char status;
} gAllocatorBlock;

/**
 * Create a new allocator
 * @param heapSize The size of the heap
 * @param defaultBlockSize The default block size
 * @return The new allocator
 */
gAllocator* gAllocatorCreate(const size_t heapSize, const size_t defaultBlockSize);

/**
 * Free the allocator
 * @param allocator The allocator to free
 */
void gAllocatorSelfFree(gAllocator* allocator);

/**
 * Allocate memory
 * @param allocator The allocator
 * @param size The size of the memory to allocate
 * @return The pointer to the allocated memory
 */
gPtr gAllocatorAlloc(const gAllocator* allocator, const size_t size);

/**
 * Free memory
 * @param allocator The allocator
 * @param ptr The pointer to the memory to free
 */
void gAllocatorFree(const gAllocator* allocator, const gPtr ptr);

/**
 * Reallocate memory
 * @param allocator The allocator
 * @param ptr The pointer to the memory to reallocate
 * @param newSize The new size of the memory
 * @return The pointer to the reallocated memory
 */
gPtr gAllocatorReAlloc(const gAllocator *allocator, const gPtr ptr, const size_t newSize);

/**
 * Copy the allocator to a different location
 * @param allocator The allocator
 * @param dest The destination
 */
void gAllocatorCopyTo(const gAllocator *allocator, void *dest);

/**
 * Copy the allocator from a different location
 * @param allocator The allocator
 * @param src The source
 */
void gAllocatorCopyFrom(gAllocator *allocator, const void *src, const size_t size);

/**
 * Private function to initialize a block
 */
void gAllocatorInitBlock(gAllocatorBlock* block, size_t size);

/**
 * Private function to mark a block as used
 */
void gAllocatorUseBlock(gAllocatorBlock* block);

/**
 * Private function to get the block of a pointer
 */
gAllocatorBlock* gAllocatorGetBlock(const gAllocator* allocator, const gPtr ptr);

#define align(ptr, alignment) ((ptr + alignment - 1) & ~(alignment - 1))
#define max(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

#endif