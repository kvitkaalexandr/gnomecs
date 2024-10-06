#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

//todo: rewrite this to use relative pointers

typedef int(*gHashFunction)(const void *key, const size_t keySize);
typedef bool(*gEqualFunction)(const void *keys1, const void *key2, const size_t keySize);

typedef struct gHashMapEntry {
    struct gHashMapEntry *next;
    void *key;
    void *value;
} gHashMapEntry;

typedef struct gHashMap {
    size_t keySize;
    size_t valueSize;
    size_t capacity;
    size_t count;
    gHashFunction hashFunction;
    gEqualFunction equalFunction;
    gHashMapEntry **buckets;
} gHashMap;

gHashMap *gHashMapCreate(size_t keySize, size_t valueSize, gHashFunction hashFunction, gEqualFunction equalFunction, size_t capacity);
void gHashMapFree(gHashMap *map);
void *gHashMapAt(gHashMap *map, const void *key);
bool gHashMapAdd(gHashMap *map, const void *key, const void *value);
bool gHashMapContains(gHashMap *map, const void *key);
bool gHashMapRemove(gHashMap *map, const void *key);

#endif //HASHMAP_H
