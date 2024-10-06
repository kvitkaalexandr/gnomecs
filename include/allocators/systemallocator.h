#ifndef SYSTEMALLOCATOR_H
#define SYSTEMALLOCATOR_H

#include "allocator.h"

/**
 * System allocator
 * This allocator uses the system allocator (malloc, realloc, free)
 */
gAllocator *gSystemAllocatorCreate();

#endif //SYSTEMALLOCATOR_H
