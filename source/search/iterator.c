#include "search/iterator.h"

#include <assert.h>
#include <allocators/helpers.h>

gEntityIterator gEntityIteratorCreate(gWorld *world, gArchetype *archetype) {
    return (gEntityIterator) {
        .world = world,
        .archetype = archetype,
        .index = -1
    };
}

int gEntityIteratorIterate(gEntityIterator *it, gEntity *outEntity) {
    assert(it->world != NULL);
    assert(it->archetype != NULL);

    if (it->index < 0) {
        it->index = it->archetype->firstEntity;
        if (it->index < 0) return gEntityIterator_finish;
    }

    const gChunkedList *statusRow = gTableRowAt(it->world->allocator, &it->archetype->components, 0);
    if (statusRow == NULL) return gEntityIterator_error;

    gEntityStatus *entityStatus;
    do {
        if (it->index >= statusRow->count || it->index >= it->archetype->lastEntity) return gEntityIterator_finish;
        entityStatus = gChunkedListAt(it->world->allocator, statusRow, it->index);
        it->index++;
    } while (!entityStatus->isAlive);

    const gEntity entity = {
        .index = it->index - 1,
        .version = entityStatus->version,
        .archetype = it->archetype->id
    };
    *outEntity = entity;

    return gEntityIterator_continue;
}

gArchetypeIterator gArchetypeIteratorCreate(gWorld *world, gQuery query) {
    return (gArchetypeIterator){
        .world = world,
        .index = 0,
        .query = query
    };
}

bool _gEntityIteratorFilterArch(const gArchetype *arch, const gQuery query) {
    bool match = true;

    if (!gBitSetEmpty(&query.all))
        match &= gBitSetAll(&arch->definition, &query.all);

    if (!gBitSetEmpty(&query.any))
        match &= gBitSetAny(&arch->definition, &query.any);

    if (!gBitSetEmpty(&query.none))
        match &= gBitSetNone(&arch->definition, &query.none);

    return match;
}

int gArchetypeIteratorIterate(gArchetypeIterator *it, gArchetype **outArchetype) {
    assert(it->world != NULL);

    if (it->world->archetypes.count == 0) return gEntityIterator_finish;

    gArchetype *archetype;
    do {
        if (it->index >= it->world->archetypes.count) return gEntityIterator_finish;
        archetype = gChunkedListAt(it->world->allocator, &it->world->archetypes, it->index);
        it->index++;
    } while (!_gEntityIteratorFilterArch(archetype, it->query));

    *outArchetype = archetype;
    return gEntityIterator_continue;
}

gAllFilteredEntitiesIterator gAllFilteredEntitiesIteratorCreate(gWorld *world, gQuery query) {
    return (gAllFilteredEntitiesIterator){
        .currentArchetype = NULL,
        .archetypeIterator = gArchetypeIteratorCreate(world, query),
        .entityIterator = gEntityIteratorCreate(world, NULL)
    };
}

int _moveToNextArchetype(gAllFilteredEntitiesIterator *it) {
    const int result = gArchetypeIteratorIterate(&it->archetypeIterator, &it->currentArchetype);
    if (result != gEntityIterator_continue) return result;
    it->entityIterator = gEntityIteratorCreate(it->archetypeIterator.world, it->currentArchetype);
    return gEntityIterator_continue;
}

int gAllFilteredEntitiesIteratorIterate(gAllFilteredEntitiesIterator *it, gEntity *outEntity) {
    assert(it->archetypeIterator.world != NULL);
    assert(it->entityIterator.world != NULL);

    if (it->currentArchetype == NULL) {
        const int result = _moveToNextArchetype(it);
        if (result != gEntityIterator_continue) return result;
    }

    while (true) {
        int result = gEntityIteratorIterate(&it->entityIterator, outEntity);
        if (result == gEntityIterator_continue) return gEntityIterator_continue;

        if (result == gEntityIterator_finish) {
            result = _moveToNextArchetype(it);
            if (result != gEntityIterator_continue) return result;
        } else {
            return result;
        }
    }

    return gEntityIterator_unknown;
}

int _moveToNextArchetypeFromCache(gAllFilteredEntitiesCacheIterator *it) {
    const gQueryCache *cache = &it->entityIterator.world->queryCache;
    const gWorld *world = it->entityIterator.world;

    if (!it->entry || it->entry->archetypesCount != world->archetypes.count) {

        gArchetypeIterator archIt = gArchetypeIteratorCreate(it->entityIterator.world, it->query);

        if (!it->entry) {
            //create new entry...
            gVector *archetypes = gAllocatorAbsAlloc(cache->allocator, sizeof(gVector));
            gVectorInit(cache->allocator, archetypes, sizeof(unsigned int), 16);

            gQueryCacheEntry *e = gAllocatorAbsAlloc(cache->allocator, sizeof(gQueryCacheEntry));
            e->archetypes = archetypes;

            gHashMapAdd(cache->data, &it->query, e);
            it->entry = gHashMapAt(cache->data, &it->query);
        }
        else {
            //update existing entry...
            archIt.index = it->entry->archetypesCount;
        }

        gArchetype *arch;
        while (gArchetypeIteratorIterate(&archIt, &arch) == gEntityIterator_continue) {
            gVectorAdd(cache->allocator, it->entry->archetypes, &arch->id);
        }

        it->entry->archetypesCount = world->archetypes.count;
    }

    int *archetypeIdx = gVectorAt(cache->allocator, it->entry->archetypes, it->cacheIdx);
    if (archetypeIdx == NULL) return gEntityIterator_finish;

    gArchetype *a = gChunkedListAt(world->allocator, &world->archetypes, *archetypeIdx);
    if (a == NULL) return gEntityIterator_finish;


    it->entityIterator = gEntityIteratorCreate(world, a);

    it->cacheIdx++;
    return gEntityIterator_continue;
}

gAllFilteredEntitiesCacheIterator gAllFilteredEntitiesCacheIteratorCreate(gWorld *world, gQuery query) {
    return (gAllFilteredEntitiesCacheIterator){
        .query = query,
        .cacheIdx = 0,
        .entry = NULL,
        .entityIterator = gEntityIteratorCreate(world, NULL)
    };
}

int gAllFilteredEntitiesCacheIteratorIterate(gAllFilteredEntitiesCacheIterator *it, gEntity *outEntity) {
    assert(it != NULL);

    if (it->entry == NULL) {
        it->entry = gHashMapAt(it->entityIterator.world->queryCache.data, &it->query);
        const int result = _moveToNextArchetypeFromCache(it);
        if (result != gEntityIterator_continue) return result;
    }

    while (true) {
        int result = gEntityIteratorIterate(&it->entityIterator, outEntity);
        if (result == gEntityIterator_continue) return gEntityIterator_continue;

        if (result == gEntityIterator_finish) {
            result = _moveToNextArchetypeFromCache(it);
            if (result != gEntityIterator_continue) return result;
        } else {
            return result;
        }
    }

    return gEntityIterator_unknown;
}
