#ifndef SLICE_H
#define SLICE_H
#include "../allocators/allocator.h"
#include <stdbool.h>

/**
 * Slice is a view of a contiguous memory block
 * This data structure was designed to be used with the gAllocator (with relative pointers)
 */
typedef struct {
    gPtr data;
    size_t count;
    size_t size;
} gSlice;

/**
 * Initialize a slice
 * @param slice Slice to initialize
 * @param ptr Relative pointer to the data
 * @param size Size of each element
 * @param count Count of elements
 */
void gSliceInit(gSlice *slice, const gPtr ptr, const size_t size, const size_t count);

/**
 * Get the element at the specified index
 * @param allocator Allocator, needed to convert the relative pointer to an absolute pointer
 * @param slice
 * @param index
 * @return
 */
void *gSliceAt(const gAllocator *allocator, const gSlice *slice, const size_t index);

/**
 * Set the element at the specified index
 * @param allocator Allocator, needed to convert the relative pointer to an absolute pointer
 * @param slice
 * @param index
 * @param element
 * @return
 */
bool gSliceSet(const gAllocator *allocator, gSlice *slice, const size_t index, const void *element);

/**
 * Free the slice
 * @param allocator Allocator, needed to convert the relative pointer to an absolute pointer
 * @param slice
 */
void gSliceFree(const gAllocator *allocator, gSlice *slice);

#endif //SLICE_H
