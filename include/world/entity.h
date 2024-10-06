#ifndef ENTITY_H
#define ENTITY_H
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    unsigned int index;
    unsigned int version; //0 - is not valid value
    unsigned short archetype;
} gEntity;

/**
 * Create entity with components
 * @param entityName
 * @param ... component ids
 */
#define _createEntity(entityName, ...) gEntity entityName = {0}; \
    { \
        gBitSet ebs = {0}; \
        unsigned int args[] = {__VA_ARGS__}; \
        gBitSetSetN(&ebs, sizeof(args) / sizeof(unsigned int), __VA_ARGS__); \
        entityName = gWorldCreateEntity(__cw, ebs); \
    }

#define gEntityToString(str, entity) sprintf(str, "Entity(i: %d, v: %d, a: %d)", entity.index, entity.version, entity.archetype);

bool gEntityEq(gEntity e1, gEntity e2);

#endif //ENTITY_H
