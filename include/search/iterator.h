#ifndef ITERATOR_H
#define ITERATOR_H
#include <stddef.h>
#include "../world/entity.h"
#include "../world/world.h"
#include "query.h"

enum {
    gEntityIterator_unknown = 0,
    gEntityIterator_error = 1,
    gEntityIterator_finish = 2,
    gEntityIterator_continue = 3
};

typedef struct {
    gWorld *world;
    gArchetype *archetype;
    int index;
} gEntityIterator;

typedef struct {
    gWorld *world;
    size_t index;
    gQuery query;
} gArchetypeIterator;

typedef struct {
    gArchetype *currentArchetype;
    gArchetypeIterator archetypeIterator;
    gEntityIterator entityIterator;
} gAllFilteredEntitiesIterator;

typedef struct {
    gQuery query;
    size_t cacheIdx;
    gQueryCacheEntry *entry;
    gEntityIterator entityIterator;
} gAllFilteredEntitiesCacheIterator;


gEntityIterator gEntityIteratorCreate(gWorld *world, gArchetype *archetype);

int gEntityIteratorIterate(gEntityIterator *it, gEntity *outEntity);

gArchetypeIterator gArchetypeIteratorCreate(gWorld *world, gQuery query);

int gArchetypeIteratorIterate(gArchetypeIterator *it, gArchetype **outArchetype);

gAllFilteredEntitiesIterator gAllFilteredEntitiesIteratorCreate(gWorld *world, gQuery query);

int gAllFilteredEntitiesIteratorIterate(gAllFilteredEntitiesIterator *it, gEntity *outEntity);

gAllFilteredEntitiesCacheIterator gAllFilteredEntitiesCacheIteratorCreate(gWorld *world, gQuery query);

int gAllFilteredEntitiesCacheIteratorIterate(gAllFilteredEntitiesCacheIterator *it, gEntity *outEntity);

#endif //ITERATOR_H
