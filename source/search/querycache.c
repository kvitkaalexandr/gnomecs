#include "search/querycache.h"

#include <assert.h>
#include "search/query.h"

int __gHashFunction(const void *key, const size_t keySize) {
    gQuery query = *(gQuery *)key;
    return gQueryGetHashCode(query);
}

bool __gEqualFunction(const void *key1, const void *key2, const size_t keySize) {
    gQuery query1 = *(gQuery *)key1;
    gQuery query2 = *(gQuery *)key2;
    return gQueryEquals(query1, query2);
}

bool gQueryCacheInit(gQueryCache *cache) {
    cache->allocator = gAllocatorCreate(1000000, 16);
    cache->data = gHashMapCreate(
        cache->allocator,
        sizeof(gQuery),
        sizeof(gQueryCacheEntry),
        __gHashFunction,
        __gEqualFunction,
        16);
    return true;
}

void gQueryCacheFree(const gQueryCache *cache) {
    assert(cache != NULL);
    gAllocatorSelfFree(cache->allocator);
}

void freeEntry(const gQueryCache *cache, const gQuery *query, const gQueryCacheEntry *entry) {
    gVectorFree(cache->allocator, entry->archetypes);
    gHashMapRemove(cache->data, query);
}