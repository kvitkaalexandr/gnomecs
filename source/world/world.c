#include "world/world.h"

#include <string.h>

void gWorldFree(gWorld *world) {
    if (world->allocator != NULL) gAllocatorSelfFree(world->allocator);
    gQueryCacheFree(&world->queryCache);
    free(world);
}

gWorld *gWorldCreate() {
    gWorld *w = malloc(sizeof(gWorld));
    if (w == NULL) return NULL;

    w->allocator = gAllocatorCreate(gWorldAllocatorSize, gWorldAllocatorBlockSize);
    if (w->allocator == NULL) {
        gWorldFree(w);
        return NULL;
    }

    if (!gComponentsDbInit(w->allocator, &w->componentsDb)) {
        gWorldFree(w);
        return NULL;
    }

    gChunkedListInit(w->allocator, &w->archetypes, sizeof(gArchetype), gWorldArchetypeInitialCapacity);
    gQueryCacheInit(&w->queryCache);

    return w;
}

gArchetype *gWorldFindArchetype(const gWorld *world, gBitSet definition) {
    //todo: currently o(n) do better
    for (size_t i = 0; i < world->archetypes.count; i++) {
        gArchetype *a = gChunkedListAt(world->allocator, &world->archetypes, i);
        if (a == NULL) continue;
        if (gBitSetEq(&a->definition, &definition)) return a;
    }

    return NULL;
}

gEntity gWorldCreateEntity(gWorld *world, const gBitSet definition) {
    gArchetype *a = gWorldFindArchetype(world, definition);
    if (a == NULL) {
        if (!gChunkedListPushBack(world->allocator, &world->archetypes, &(gArchetype){0})) return (gEntity){0};
        const int aId = world->archetypes.count - 1;
        a = gChunkedListAt(world->allocator, &world->archetypes, aId);
        gArchetypeInit(world->allocator, a, &world->componentsDb, definition, aId);
    }
    world->version++;
    return gArchetypeCreateEntity(world->allocator, a);
}

bool gWorldDestroyEntity(gWorld *world, const gEntity e) {
    gArchetype *a = gChunkedListAt(world->allocator, &world->archetypes, e.archetype);
    if (a == NULL) return false;
    const bool r = gArchetypeDestroyEntity(world->allocator, a, e);
    if (r) world->version++;
    return r;
}

bool gWorldIsEntityAlive(const gWorld *world, const gEntity e) {
    const gArchetype *a = gChunkedListAt(world->allocator, &world->archetypes, e.archetype);
    if (a == NULL) return false;
    return gArchetypeIsEntityAlive(world->allocator, a, e);
}

void *gWorldGetComponent(const gWorld *world, const gEntity e, const unsigned int componentId) {
    const gArchetype *a = gChunkedListAt(world->allocator, &world->archetypes, e.archetype);
    if (a == NULL) return NULL;
    return gArchetypeGetComponent(world->allocator, a, e, componentId);
}

int gWorldRegComponent(gWorld *world, const gComponentData component) {
    return gComponentsDbAdd(world->allocator, &world->componentsDb, &component);
}

bool gWorldHasComponent(const gWorld *world, const gEntity e, const unsigned int componentId) {
    const gArchetype *arch = gChunkedListAt(world->allocator, &world->archetypes, e.archetype);
    if (arch == NULL) return false;
    gBitSet bs = {0};
    gBitSetSet(&bs, componentId);
    return gBitSetAll(&arch->definition, &bs);
}

void gWorldCopyTo(const gWorld *world, void *to) {
    gAllocatorCopyTo(world->allocator, to);
}

void gWorldCopyFrom(gWorld *world, const void *from, const size_t size) {
    gAllocatorCopyFrom(world->allocator, from, size);
    gQueryCacheReset(&world->queryCache);
}
