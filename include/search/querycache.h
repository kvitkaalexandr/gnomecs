#ifndef QUERYCACHE_H
#define QUERYCACHE_H

#include "collections/absolute/hashmap.h"
#include "collections/vector.h"

typedef struct {
    gAllocator *allocator;
    gHashMap *data;
} gQueryCache;

typedef struct {
    int archetypesCount;
    gVector *archetypes;
} gQueryCacheEntry;

bool gQueryCacheInit(gQueryCache *cache);
void gQueryCacheFree(const gQueryCache *cache);

#endif //QUERYCACHE_H
