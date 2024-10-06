#include "collections/table.h"

bool gTableInit(const gAllocator *allocator, gTable *table, const size_t chunkSize, const size_t initialRowCapacity) {
    table->chunkSize = chunkSize;
    return gVectorInit(allocator, &table->rows, sizeof(gChunkedList), initialRowCapacity);
}

gChunkedList *gTableRowAt(const gAllocator *allocator, const gTable *table, const size_t index) {
    return gVectorAt(allocator, &table->rows, index);
}

void gTableFree(const gAllocator *allocator, gTable *table) {
    for (size_t i = 0; i < table->rows.count; i++) {
        gChunkedList *chunkedList = gTableRowAt(allocator, table, i);
        gChunkedListFree(allocator, chunkedList);
    }
    gVectorFree(allocator, &table->rows);
}

bool gTableAddRow(const gAllocator *allocator, gTable *table, const size_t elementSize) {
    gChunkedList chunkedList;
    if (!gChunkedListInit(allocator, &chunkedList, elementSize, table->chunkSize)) return false;
    return gVectorAdd(allocator, &table->rows, &chunkedList);
}