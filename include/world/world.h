#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "../archetype/archetype.h"
#include "entity.h"
#include "../components/componentsdb.h"

#define gWorldAllocatorSize 16000000
#define gWorldAllocatorBlockSize 8
#define gWorldArchetypeInitialCapacity 16

typedef struct {
    gAllocator *allocator;
    gChunkedList archetypes;
    gComponentsDb componentsDb;
    int version;
} gWorld;

gWorld *gWorldCreate();

void gWorldFree(gWorld *world);

gArchetype *gWorldFindArchetype(const gWorld *world, gBitSet definition);

gEntity gWorldCreateEntity(gWorld *world, const gBitSet definition);

bool gWorldDestroyEntity(gWorld *world, const gEntity e);

bool gWorldIsEntityAlive(const gWorld *world, const gEntity e);

void *gWorldGetComponent(const gWorld *world, const gEntity e, const unsigned int componentId);

int gWorldRegComponent(gWorld *world, const gComponentData component);

bool gWorldHasComponent(const gWorld *world, const gEntity e, const unsigned int componentId);

void gWorldCopyTo(const gWorld *world, void *to);

void gWorldCopyFrom(gWorld *world, const void *from, const size_t size);

#endif
