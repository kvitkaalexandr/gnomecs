#include "components/componentsdb.h"
#include "components/components.h"

bool gComponentsDbInit(const gAllocator *alloc, gComponentsDb *db) {
    return gVectorInit(alloc, &db->data, sizeof(gComponentData), gComponentsMax);
}

int gComponentsDbAdd(const gAllocator *alloc, gComponentsDb *db, const gComponentData *componentData) {
    if (db->data.count >= gComponentsMax) return -1;
    gVectorAdd(alloc, &db->data, componentData);
    return db->data.count - 1;
}
