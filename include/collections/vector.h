#ifndef VECTOR_H
#define VECTOR_H

#include "../allocators/allocator.h"
#include "slice.h"
#include <stdbool.h>

/**
 * Vector is a dynamic array
 * This data structure was designed to be used with the gAllocator (with relative pointers)
 */
typedef struct {
    gSlice data;
    size_t capacity;
    size_t count;
    size_t elementSize;
} gVector;

/**
 * Initialize a vector
 * @param allocator Allocator
 * @param vector Vector to initialize
 * @param elementSize Size of each element
 * @param initialCapacity Initial capacity
 * @return
 */
bool gVectorInit(gAllocator *allocator, gVector *vector, const size_t elementSize, const size_t initialCapacity);

/**
 * Add an element to the vector
 * @param allocator Allocator
 * @param vector Vector
 * @param element Element to add
 * @return
 */
bool gVectorAdd(gAllocator *allocator, gVector *vector, const void *element);

/**
 * Remove an element from the vector
 * @param allocator Allocator
 * @param vector Vector
 * @param index Index of the element to remove
 * @return
 */
bool gVectorRemove(const gAllocator *allocator, gVector *vector, const size_t index);

/**
 * Set the element at the specified index
 * @param allocator Allocator
 * @param vector Vector
 * @param index Index of the element to set
 * @param element Element to set
 * @return
 */
bool gVectorSet(const gAllocator *allocator, gVector *vector, const size_t index, const void *element);

/**
 * Get the element at the specified index
 * @param allocator Allocator
 * @param vector Vector
 * @param index Index of the element to get
 * @return Absolute pointer to the element
 */
void *gVectorAt(const gAllocator *allocator, const gVector *vector, const size_t index);

/**
 * Clear the vector
 * @param vector Vector
 */
void gVectorClear(gVector *vector);

/**
 * Free the vector
 * @param allocator Allocator
 * @param vector Vector
 */
void gVectorFree(gAllocator *allocator, gVector *vector);

#endif //VECTOR_H
