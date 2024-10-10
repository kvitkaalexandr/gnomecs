#ifndef HELPERS_H
#define HELPERS_H

#define gAllocatorAbsAlloc(allocator, size) allocator->relToAbs(allocator, allocator->alloc(allocator, size))
#define gAllocatorAbsReAlloc(allocator, ptr, newSize) allocator->relToAbs(allocator, allocator->reAlloc(allocator, ptr, newSize))
#define gAllocatorAbsFree(allocator, ptr) allocator->free(allocator, allocator->absToRel(allocator, ptr))

#endif //HELPERS_H
