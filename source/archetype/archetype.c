#include "archetype/archetype.h"
#include "components/components.h"
#include "log/log.h"

#define logTag "archetype"

bool gArchetypeInit(gAllocator *alloc, gArchetype *archetype, gComponentsDb *db, gBitSet definition, unsigned int id) {
    archetype->definition = definition;
    archetype->id = id;
    archetype->firstEntity = -1;
    archetype->lastEntity = 0;
    const unsigned int compCount = gBitSetCount(&definition);

    gVectorInit(alloc, &archetype->free, sizeof(size_t), 32);

    const gPtr mapPtr = gAllocatorAlloc(alloc, sizeof(size_t) * gComponentsMax);
    gSliceInit(&archetype->map, mapPtr, sizeof(size_t), gComponentsMax);

    gTableInit(alloc, &archetype->components, gEntitiesPerChunk, compCount);
    gTableAddRow(alloc, &archetype->components, sizeof(gEntityStatus));

    int initedComponent = 0;
    //todo fit gBitSet to gComponentsMax
    for (size_t i = 0; i < gComponentsMax; i++) {
        if (initedComponent >= compCount) break;
        if (!gBitSetIsSet(&definition, i)) continue;
        gComponentData *componentData = gVectorAt(alloc, &db->data, i);
        if (componentData == NULL) {
            gLogError("Component with id %d not found", logTag, i);
            return false;
        }
        if (componentData->size == 0) continue; //tag component without data
        gTableAddRow(alloc, &archetype->components, componentData->size);
        size_t *toRow = gSliceAt(alloc, &archetype->map, i);
        *toRow = archetype->components.rows.count - 1;
        initedComponent++;
    }

    gLogVerbose("New archetype created. Archetype id: %d, Components count: %d", logTag, id, compCount);
    return true;
}

gEntity gArchetypeCreateEntity(gAllocator *alloc, gArchetype *archetype) {
    gChunkedList *statusRow = gTableRowAt(alloc, &archetype->components, 0);

    gEntity e = {0};
    e.archetype = archetype->id;

    if (archetype->free.count > 0) {
        const int *freeIdx = gVectorAt(alloc, &archetype->free, archetype->free.count - 1);

        gEntityStatus *status = gChunkedListAt(alloc, statusRow, *freeIdx);
        status->version++;
        status->isAlive = true;

        e.index = *freeIdx;
        e.version = status->version;

        gVectorRemove(alloc, &archetype->free, archetype->free.count - 1);

        gLogVerbose("Reuse entity. Archetype id: %d, Entity index: %d, Entity version: %d", logTag, e.archetype, e.index, e.version);
    }
    else {
        gEntityStatus status;
        status.version = 1;
        status.isAlive = true;
        gChunkedListPushBack(alloc, statusRow, &status);

        for (size_t i = 1, max = archetype->components.rows.count; i < max; i++) {
            gChunkedList *row = gTableRowAt(alloc, &archetype->components, i);
            gChunkedListPushBackDefault(alloc, row);
        }

        e.index = statusRow->count - 1;
        e.version = status.version;

        gLogVerbose("Create new entity. Archetype id: %d, Entity index: %d, Entity version: %d", logTag, e.archetype, e.index, e.version);
    }

    if (archetype->firstEntity < 0 || e.index < archetype->firstEntity) {
        archetype->firstEntity = e.index;
    }
    if (e.index >= archetype->lastEntity) {
        archetype->lastEntity = e.index + 1;
    }

    return e;
}

bool gArchetypeDestroyEntity(const gAllocator *alloc, gArchetype *archetype, const gEntity e) {
    const gChunkedList *statusRow = gTableRowAt(alloc, &archetype->components, 0);
    gEntityStatus *status = gChunkedListAt(alloc, statusRow, e.index);
    if (status == NULL) {
        gLogError("Entity not found. Archetype id: %d, Entity index: %d, Entity version: %d", logTag, e.archetype, e.index, e.version);
        return false;
    }
    if (!status->isAlive) {
        gLogError("Entity already dead. Archetype id: %d, Entity index: %d, Entity version: %d", logTag, e.archetype, e.index, e.version);
        return false;
    }

    if (status->version != e.version) {
        gLogError("Entity version mismatch. Archetype id: %d, Entity index: %d, Entity version: %d", logTag, e.archetype, e.index, e.version);
        return false;
    }

    status->isAlive = false;
    gVectorAdd(alloc, &archetype->free, &e.index);

    gLogVerbose("Destroy entity. Archetype id: %d, Entity index: %d, Entity version: %d", logTag, e.archetype, e.index, e.version);

    if (e.index == archetype->firstEntity) archetype->firstEntity++;
    if (e.index == archetype->lastEntity - 1) archetype->lastEntity--;
    if (archetype->firstEntity >= archetype->lastEntity) {
        archetype->firstEntity = -1;
        archetype->lastEntity = 0;
    }

    return true;
}

bool gArchetypeIsEntityAlive(const gAllocator *alloc, const gArchetype *archetype, const gEntity e) {
    const gChunkedList *statusRow = gTableRowAt(alloc, &archetype->components, 0);
    const gEntityStatus *status = gChunkedListAt(alloc, statusRow, e.index);
    if (status == NULL) return false;
    return status->isAlive && status->version == e.version;
}

void *gArchetypeGetComponent(const gAllocator *alloc, const gArchetype *archetype, const gEntity e, size_t componentId) {
    const size_t *toRow = gSliceAt(alloc, &archetype->map, componentId);
    const gChunkedList *row = gTableRowAt(alloc, &archetype->components, *toRow);
    if (row == NULL) return NULL;
    return gChunkedListAt(alloc, row, e.index);
}
