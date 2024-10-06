#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include "../collections/bitset.h"
#include "../collections/table.h"
#include "../components/componentsdb.h"
#include "../world/entity.h"

#define gEntitiesPerChunk 128

typedef struct {
    unsigned int version;
    bool isAlive;
} gEntityStatus;

typedef struct {
    gBitSet definition;             //bitSet with component ids
    gTable components;
    gVector free;
    gSlice map;
    unsigned int id;
} gArchetype;

bool gArchetypeInit(gAllocator *alloc, gArchetype *archetype, gComponentsDb *db, gBitSet definition, unsigned int id);

gEntity gArchetypeCreateEntity(gAllocator *alloc, gArchetype *archetype);

bool gArchetypeDestroyEntity(const gAllocator *alloc, gArchetype *archetype, const gEntity e);

bool gArchetypeIsEntityAlive(const gAllocator *alloc, const gArchetype *archetype, const gEntity e);

void *gArchetypeGetComponent(const gAllocator *alloc, const gArchetype *archetype, const gEntity e, size_t componentId);
#endif //ARCHETYPE_H
