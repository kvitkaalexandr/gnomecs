#include "search/iterator.h"

#include <assert.h>

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

bool _gEntityIteratorNextArch(const gWorld *world, gEntityIterator *it, gArchetype **oArch, gChunkedList **oStatus) {
    do {
        it->archetype++;
        if (it->archetype >= world->archetypes.count) return false;
        *oArch = gChunkedListAt(world->allocator, &world->archetypes, it->archetype);
        *oStatus = gTableRowAt(world->allocator, &(*oArch)->components, 0);
    } while(!_gEntityIteratorFilterArch(*oArch, it->query));

    return true;
}

void gEntityIteratorInit(const gWorld *world, gEntityIterator *it, gQuery query) {
    it->archetype = 0;
    it->index = 0;
    it->query = query;
    it->worldVersion = world->version;

    if (world->archetypes.count == 0) return;

    gArchetype *arch = gChunkedListAt(world->allocator, &world->archetypes, it->archetype);
    gChunkedList *statusRow = gTableRowAt(world->allocator, &arch->components, 0);
    while (!_gEntityIteratorFilterArch(arch, it->query)) {
        if (!_gEntityIteratorNextArch(world, it, &arch, &statusRow)) break;
    }
}

//todo: Add archetypes cache. Currently, we iterate over all chunks each time.
//todo: Make archetypes lookup better than o(n)
bool gEntityIteratorNext(const gWorld *world, gEntityIterator *it, gEntity *outEntity) {
    /** When we create a new entity inside the iteration, we will have unpredictable behavior
     * 1 case, we can iterate this new entity (if allocate a new or free entity ahead current iteration position)
     * 2 case we can't iterate this new entity (if free entity behind iteration position)
    */
    //todo: thik about it
    //assert(world->version == it->worldVersion);
    const gEntity invalidEntity = {0};
    *outEntity = invalidEntity;

    if (world->archetypes.count == 0) return false;
    gArchetype *arch = gChunkedListAt(world->allocator, &world->archetypes, it->archetype);
    if (arch == NULL) return false;
    gChunkedList *statusRow = gTableRowAt(world->allocator, &arch->components, 0);

    if (it->index >= statusRow->count) {
        it->index = 0;
        if (!_gEntityIteratorNextArch(world, it, &arch, &statusRow)) return false;
    }

    const gEntityStatus *status = gChunkedListAt(world->allocator, statusRow, it->index);
    if (status == NULL) return false;
    while (!status->isAlive) {
        it->index++;
        if (it->index >= statusRow->count) {
            it->index = 0;
            if (!_gEntityIteratorNextArch(world, it, &arch, &statusRow)) return false;
        }
        status = gChunkedListAt(world->allocator, statusRow, it->index);
    }

    const gEntity nextEntity = {
        .index = it->index,
        .version = status->version,
        .archetype = it->archetype
    };
    *outEntity = nextEntity;

    it->index++;

    return true;
}