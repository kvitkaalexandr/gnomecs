#ifndef CHUNKED_LIST_H
#define CHUNKED_LIST_H
#include "../allocators/allocator.h"
#include "vector.h"

/**
 * This data structure similar to a vector, but it stores elements in chunks
 * When a chunk is full, a new chunk is allocated without moving the existing elements
 *
 * This data structure was designed to be used with the gAllocator (with relative pointers)
 */
typedef struct {
    gVector chunkPointers; // gVector of gSlice
    size_t count;
    size_t chunkSize;
    size_t elementSize;
} gChunkedList;
#define gChunkedListPointerCapacity 2

/**
 * Initialize a chunked list
 * @param allocator Allocator
 * @param chunkedList Chunked list to initialize
 * @param elementSize Size of each element
 * @param chunkSize Size of each chunk
 * @return
 */
bool gChunkedListInit(const gAllocator *allocator, gChunkedList *chunkedList, const size_t elementSize, const size_t chunkSize);

/**
 * Free the chunked list
 * @param allocator Allocator
 * @param chunkedList Chunked list to free
 */
void gChunkedListFree(const gAllocator *allocator, gChunkedList *chunkedList);

/**
 * Push back default element (0) to the chunked list
 * @param allocator Allocator
 * @param chunkedList Chunked list
 * @return
 */
bool gChunkedListPushBackDefault(gAllocator *allocator, gChunkedList *chunkedList);

/**
 * Push back an element to the chunked list
 * @param allocator Allocator
 * @param chunkedList Chunked list
 * @param element Element to push back
 * @return
 */
bool gChunkedListPushBack(gAllocator *allocator, gChunkedList *chunkedList, const void *element);

/**
 * Get element at index
 * @param allocator Allocator
 * @param chunkedList Chunked list
 * @param index Index
 * @return
 */
void *gChunkedListAt(const gAllocator *allocator, const gChunkedList *chunkedList, const size_t index);

/**
 * Set element at index
 * @param allocator Allocator
 * @param chunkedList Chunked list
 * @param index Index
 * @param element Element
 * @return
 */
bool gChunkedListSet(const gAllocator *allocator, const gChunkedList *chunkedList, const size_t index, const void *element);

/**
 * Private function to allocate a new chunk
 */
bool gChunkedListAllocChunk(gAllocator *allocator, gChunkedList *chunkedList);

/**
 * Private function to push back default element (0) to the chunked list
 */
bool __gChunkedListPushBackDefault(gAllocator *allocator, gChunkedList *chunkedList, const size_t head);

#endif //CHUNKED_LIST_H
