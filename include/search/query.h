#ifndef QUERY_H
#define QUERY_H

#include "../collections/bitset.h"

typedef struct {
    gBitSet all;
    gBitSet any;
    gBitSet none;
} gQuery;

/**
 * Setup default world to query builder
 */
#define _useWorld(world) gWorld *__cw = world;

/**
 * Setup default entity to some shortcut
 * Example:
 * _useEntity(myEntity);
 * bool myEntityHasFoo = _has(Foo);
 */
#define _useEntity(entity) gEntity __entity = entity;

/**
 * Change default entity to some shortcut
 */
#define _reuseEntity(entity) __entity = entity;

/**
 * Begin query construction
 * Use other marcos like _with, _without, _foreach to define query
 */
#define _makeQuery gQuery __q = {0};

/**
 * Use query definition from variable
 */
#define _useQuery(query) gQuery __q = query;

/**
 * Save query definition to variable
 * Example:
 * _makeQuery
 *      _with(Foo)
 * gQuery mySavedQuery = _saveQuery
 */
#define _saveQuery __q;

/**
 * Entity should have all components
 */
#define _with(component) gBitSetSet(&__q.all, _componentId(component));

/**
 * Entity should have any of the components
 */
#define _withAny(component) gBitSetSet(&__q.any, _componentId(component));

/**
 * Entity should not have the component
 */
#define _without(component) gBitSetSet(&__q.none, _componentId(component));

/**
 * Iterate over entities that match the query
 *
 * __entity in the loop is the current entity
 *
 * Example:
* _makeQuery
 *     _with(Foo)
 * _foreach {
 *     printf("Entity %d has Foo\n", __entity.id);
 * }
 */
#ifdef DISABLE_CACHE
#define _foreach gAllFilteredEntitiesIterator __iter = gAllFilteredEntitiesIteratorCreate(__cw, __q); \
    gEntity __entity = {0}; \
    while(gAllFilteredEntitiesIteratorIterate(&__iter, &__entity) == gEntityIterator_continue)
#else
#define _foreach gAllFilteredEntitiesCacheIterator __iter = gAllFilteredEntitiesCacheIteratorCreate(__cw, __q); \
    gEntity __entity = {0}; \
    while(gAllFilteredEntitiesCacheIteratorIterate(&__iter, &__entity) == gEntityIterator_continue)
#endif

/**
 * Get component from the current entity
 *
 * Example:
 * _makeQuery
 *     _with(Foo)
 * _foreach {
 *     Foo *foo = _take(Foo);
 *     printf("Entity %d has Foo with value %d\n", __entity.id, foo->value);
 * }
 */
#define _take(component) ((component*)gWorldGetComponent(__cw, __entity, _componentId(component)))

/**
 * Check if entity has component
 *
 * Example:
 * _makeQuery
 *     _with(Foo)
 * _foreach {
 *     if (_has(Bar)) {
 *         printf("Entity %d has Bar\n", __entity.id);
 *     }
 * }
 */
#define _has(component) gWorldHasComponent(__cw, __entity, _componentId(component))

#ifdef DISABLE_CACHE
#define _single(to, component) component *to = NULL; \
{ \
    gQuery __q = {0}; \
    gBitSetSet(&__q.all, _componentId(component)); \
    gAllFilteredEntitiesIterator __iter = gAllFilteredEntitiesIteratorCreate(__cw, __q); \
    gEntity e = {0}; \
    gAllFilteredEntitiesIteratorIterate(&__iter,  &e); \
    to = (component*)gWorldGetComponent(__cw, e, _componentId(component)); \
}
#define _singleEntity(to, component) gEntity to = {0}; \
{ \
    gQuery __q = {0}; \
    gBitSetSet(&__q.all, _componentId(component)); \
    gAllFilteredEntitiesIterator __iter = gAllFilteredEntitiesIteratorCreate(__cw, __q); \
    gAllFilteredEntitiesIteratorIterate(&__iter, &to); \
}
#else
#define _single(to, component) component *to = NULL; \
{ \
    gQuery __q = {0}; \
    gBitSetSet(&__q.all, _componentId(component)); \
    gAllFilteredEntitiesCacheIterator __iter = gAllFilteredEntitiesCacheIteratorCreate(__cw, __q); \
    gEntity e = {0}; \
    gAllFilteredEntitiesCacheIteratorIterate(&__iter,  &e); \
    to = (component*)gWorldGetComponent(__cw, e, _componentId(component)); \
}

#define _singleEntity(to, component) gEntity to = {0}; \
{ \
    gQuery __q = {0}; \
    gBitSetSet(&__q.all, _componentId(component)); \
    gAllFilteredEntitiesCacheIterator __iter = gAllFilteredEntitiesCacheIteratorCreate(__cw, __q); \
    gAllFilteredEntitiesCacheIteratorIterate(&__iter, &to); \
}
#endif

int gQueryGetHashCode(gQuery query);
bool gQueryEquals(gQuery a, gQuery b);

#endif //QUERY_H
