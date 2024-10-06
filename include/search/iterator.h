#ifndef ITERATOR_H
#define ITERATOR_H
#include <stddef.h>
#include "../world/entity.h"
#include "../world/world.h"
#include "query.h"

typedef struct {
    size_t archetype;
    size_t index;
    gQuery query;
    int worldVersion;
} gEntityIterator;

typedef struct {
    unsigned int id;
    size_t size;
} gComponentType;

void gEntityIteratorInit(const gWorld *world, gEntityIterator *it, gQuery query);

bool gEntityIteratorNext(const gWorld *world, gEntityIterator *it, gEntity *outEntity);

bool _gEntityIteratorFilterArch(const gArchetype *arch, const gQuery query);

bool _gEntityIteratorNextArch(const gWorld *world, gEntityIterator *it, gArchetype **oArch, gChunkedList **oStatus);

#endif //ITERATOR_H
