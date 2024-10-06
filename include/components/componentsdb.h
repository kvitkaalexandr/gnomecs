#ifndef COMPONENTS_DB_H
#define COMPONENTS_DB_H

#include "../collections/vector.h"
#include "../allocators/allocator.h"

typedef struct {
    size_t size;
} gComponentData;

typedef struct {
    gVector data;
} gComponentsDb;

bool gComponentsDbInit(const gAllocator *alloc, gComponentsDb *db);

int gComponentsDbAdd(const gAllocator *alloc, gComponentsDb *db, const gComponentData *componentData);

#endif //COMPONENTSREGESTRY_H
