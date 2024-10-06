#ifndef TABLE_H
#define TABLE_H
#include "chunkedlist.h"
#include "vector.h"

/**
 * Columns are chunked lists
 * Rows are vectors, each row can store a different type of data
 * This data structure was designed for ecs archetypes
 *
 * This data structure was designed to be used with the gAllocator (with relative pointers)
 */
typedef struct {
    gVector rows; // gVector of gChunkedList
    size_t chunkSize;
} gTable;

/**
 * Initialize a table
 * @param allocator Allocator
 * @param table Table to initialize
 * @param chunkSize Size of each chunk (column)
 * @param initialRowCapacity Initial row capacity
 * @return
 */
bool gTableInit(const gAllocator *allocator, gTable *table, const size_t chunkSize, const size_t initialRowCapacity);

/**
 * Free the table
 * @param allocator Allocator
 * @param table Table to free
 */
void gTableFree(const gAllocator *allocator, gTable *table);

/**
 * Get row at index
 * @param allocator Allocator
 * @param table Table
 * @param index Index
 * @return absolute pointer to the row
 */
gChunkedList *gTableRowAt(const gAllocator *allocator, const gTable *table, const size_t index);

/**
 * Add a row to the table
 * @param allocator Allocator
 * @param table Table
 * @param elementSize Size of each element
 * @return
 */
bool gTableAddRow(const gAllocator *allocator, gTable *table, const size_t elementSize);

#endif //TABLE_H
