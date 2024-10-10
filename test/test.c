#include <stdio.h>
#include <assert.h>
#include <stdalign.h>
#include <string.h>
#include <log/log.h>

#include "allocators/allocator.h"
#include "collections/bitset.h"
#include "archetype/archetype.h"
#include "world/world.h"
#include "search/query.h"
#include "search/iterator.h"
#include "components/components.h"
#include "collections/absolute/hashmap.h"
#include "allocators/systemallocator.h"

/**
 * Test not finished. Just a temporary code to test the new features etc
 */

typedef struct {
    int i;
    char c;
} testStruct;

typedef struct {
    int i;
} testComponent1;

typedef struct {
    char c;
} testComponent2;

typedef struct {
    int i;
    char c;
} testComponent3;

#define testGeneral(actual, expected, format) {if (actual != expected) {printf(format, expected, actual); printf("at %s:%d", __FILE__, __LINE__); exit(EXIT_FAILURE);}}
#define testInt(actual, expected) testGeneral(actual, expected, "Expected %d but got %d\n")
#define testFloat(actual, expected) testGeneral(actual, expected, "Expected %f but got %f\n")
#define testChar(actual, expected) testGeneral(actual, expected, "Expected %c but got %c\n")
#define testBool(actual, expected) testGeneral(actual, expected, "Expected %d but got %d\n")
#define runTestCase(testCase) {printf("%s: %s\n", #testCase, "Begin testing..."); testCase(); printf("%s\n\n", "Success!");}

void allocatorTest() {
    int initialSize = 64000;

    gAllocator* alloc = gAllocatorCreate(initialSize, 4096);
    assert(alloc != NULL);
    assert(alloc->heap != NULL);
    assert((long)&alloc->heap % alignof(max_align_t) == 0);
    assert(gAllocatorGetState(alloc)->size == initialSize);
    assert(gAllocatorGetState(alloc)->freeBlock == gNull);

    gAllocatorState* allocState = gAllocatorGetState(alloc);

    gPtr ts1 = gAllocatorAlloc(alloc, sizeof(testStruct));
    assert(ts1 != gNull);
    testStruct* tsp1 = gPtrToAbsPtr(alloc, ts1);
    assert((char*)tsp1 < (char*)gPtrToAbsPtr(alloc, allocState->size));
    tsp1->i = 42;
    tsp1->c = 'a';

    gPtr ts2 = gAllocatorAlloc(alloc, sizeof(testStruct));
    assert(ts2 != gNull);
    testStruct* tsp2 = gPtrToAbsPtr(alloc, ts2);
    tsp2->i = 69;
    tsp2->c = 'b';

    assert(tsp1->i == 42);
    assert(tsp1->c == 'a');
    assert(tsp2->i == 69);
    assert(tsp2->c == 'b');

    void* hp = alloc->heap;
    void* tsHp1 = hp + ts1;

    memset(tsHp1, 0, sizeof(testStruct));

    assert(tsp1->i == 0);
    assert(tsp1->c == 0);

    gPtr headBeforeHeap = allocState->head;
    gAllocatorFree(alloc, ts1);
    gAllocatorAlloc(alloc, sizeof(testStruct));
    assert(allocState->head == headBeforeHeap);
    gAllocatorAlloc(alloc, sizeof(testStruct));
    assert(allocState->head != headBeforeHeap);

    gAllocatorSelfFree(alloc);
}


void gBitSetTest() {
    gBitSet bs = {0};
    gBitSetSet(&bs, 2);
    assert(gBitSetIsSet(&bs, 2));
    assert(!gBitSetIsSet(&bs, 3));
    gBitSetClear(&bs, 2);
    assert(!gBitSetIsSet(&bs, 2));

    gBitSet otherBs = {0};
    gBitSetSet(&bs, 2);
    gBitSetSet(&otherBs, 2);
    gBitSetSet(&otherBs, 3);
    assert(gBitSetAny(&bs, &otherBs));
    assert(!gBitSetAll(&bs, &otherBs));
    gBitSetSet(&bs, 3);
    assert(gBitSetAll(&bs, &otherBs));
    bs = (gBitSet){0};
    assert(!gBitSetAny(&bs, &otherBs));
}

void gSliceTestInternal(gAllocator *alloc) {
    gPtr ptr = alloc->alloc(alloc, 32);
    gSlice slice;
    gSliceInit(&slice, ptr, 1, 16);

    char *a = gSliceAt(alloc, &slice, 0);
    *a = 'a';
    char *b = gSliceAt(alloc, &slice, 15);
    *b = 'b';

    char *ra = alloc->relToAbs(alloc, ptr);
    char *rb = alloc->relToAbs(alloc, ptr + 15);
    assert(*ra == 'a');
    assert(*rb == 'b');

    char *nPtr = gSliceAt(alloc, &slice, 16);
    assert(nPtr == NULL);
}

void gSliceTest() {
    gAllocator *alloc = gAllocatorCreate(1024, 256);
    gSliceTestInternal(alloc);
    gAllocatorSelfFree(alloc);

    gAllocator *sysAlloc = gSystemAllocatorCreate();
    gSliceTestInternal(sysAlloc);
    gAllocatorSelfFree(sysAlloc);
}

void gVectorTestInternal(gAllocator *alloc) {
    gVector vector;
    gVectorInit(alloc, &vector, sizeof(int), 1);

    for (int i = 0; i < 16; i++) {
        gVectorAdd(alloc, &vector, &i);
    }
    assert(vector.count == 16);

    for (int i = 0; i < 16; i++) {
        int *x = gVectorAt(alloc, &vector, i);
        assert(*x == i);
    }

    const int c42 = 42;
    gVectorSet(alloc, &vector, 0, &c42);
    int *i0 = gVectorAt(alloc, &vector, 0);
    assert(*i0 == c42);

    bool r = gVectorSet(alloc, &vector, 16, &c42);
    assert(!r);

    gVectorRemove(alloc, &vector, 0);
    assert(vector.count == 15);
    assert(*i0 == 1);

    gVectorRemove(alloc, &vector, vector.count - 1);
    assert(vector.count == 14);

    gVectorClear(&vector);
    assert(vector.count == 0);

    i0 = gVectorAt(alloc, &vector, 0);
    assert(i0 == NULL);

    gVectorFree(alloc, &vector);
}

void gVectorTest() {
    gAllocator *alloc = gAllocatorCreate(16000, 32);
    gVectorTestInternal(alloc);
    gAllocatorSelfFree(alloc);

    gAllocator *sysAlloc = gSystemAllocatorCreate();
    gVectorTestInternal(sysAlloc);
    gAllocatorSelfFree(sysAlloc);
}

void gChunkedListTestInternal(gAllocator *alloc) {
    gChunkedList chunkedList;
    gChunkedListInit(alloc, &chunkedList, sizeof(int), 4);

    //fill first chunk
    for (int i = 0; i < 4; i++) {
        gChunkedListPushBack(alloc, &chunkedList, &i);
    }
    for (int i = 0; i < 4; i++) {
        int *x = gChunkedListAt(alloc, &chunkedList, i);
        testInt(*x, i);
    }

    //will be allocated new chunk
    for (int i = 4; i < 8; i++) {
        gChunkedListPushBack(alloc, &chunkedList, &i);
    }
    for (int i = 0; i < 8; i++) {
        int *x = gChunkedListAt(alloc, &chunkedList, i);
        testInt(*x, i);
    }

    int newVal = 42;
    gChunkedListSet(alloc, &chunkedList, 2, &newVal);
    int *x = gChunkedListAt(alloc, &chunkedList, 2);
    testInt(*x, newVal);

    gChunkedListFree(alloc, &chunkedList);
}

void gChunkedListTest() {
    gAllocator *alloc = gAllocatorCreate(16000, 32);
    gChunkedListTestInternal(alloc);
    gAllocatorSelfFree(alloc);

    gAllocator *sysAlloc = gSystemAllocatorCreate();
    gChunkedListTestInternal(sysAlloc);
    gAllocatorSelfFree(sysAlloc);
}

void gTableTestInternal(gAllocator *alloc) {
    gTable table;
    gTableInit(alloc, &table, 4, 4);

    gTableAddRow(alloc, &table, sizeof(int));
    gTableAddRow(alloc, &table, sizeof(float));
    gTableAddRow(alloc, &table, sizeof(char));

    gChunkedList *intRow = gTableRowAt(alloc, &table, 0);
    gChunkedList *floatRow = gTableRowAt(alloc, &table, 1);
    gChunkedList *charLow = gTableRowAt(alloc, &table, 2);

    int i1 = 42;
    int i2 = 69;
    int i3 = 420;
    gChunkedListPushBack(alloc, intRow, &i1);
    gChunkedListPushBack(alloc, intRow, &i2);
    gChunkedListPushBack(alloc, intRow, &i3);

    float f1 = 42.5f;
    float f2 = 69.5f;
    float f3 = 420.5f;
    gChunkedListPushBack(alloc, floatRow, &f1);
    gChunkedListPushBack(alloc, floatRow, &f2);
    gChunkedListPushBack(alloc, floatRow, &f3);

    char c1 = 'a';
    char c2 = 'b';
    char c3 = 'c';
    gChunkedListPushBack(alloc, charLow, &c1);
    gChunkedListPushBack(alloc, charLow, &c2);
    gChunkedListPushBack(alloc, charLow, &c3);

    testInt(*(int*)gChunkedListAt(alloc, intRow, 0), i1);
    testInt(*(int*)gChunkedListAt(alloc, intRow, 1), i2);
    testInt(*(int*)gChunkedListAt(alloc, intRow, 2), i3);
    testFloat(*(float*)gChunkedListAt(alloc, floatRow, 0), f1);
    testFloat(*(float*)gChunkedListAt(alloc, floatRow, 1), f2);
    testFloat(*(float*)gChunkedListAt(alloc, floatRow, 2), f3);
    testChar(*(char*)gChunkedListAt(alloc, charLow, 0), c1);
    testChar(*(char*)gChunkedListAt(alloc, charLow, 1), c2);
    testChar(*(char*)gChunkedListAt(alloc, charLow, 2), c3);

    gTableFree(alloc, &table);
}

void gTableTest() {
    gAllocator *alloc = gAllocatorCreate(16000, 32);
    gTableTestInternal(alloc);
    gAllocatorSelfFree(alloc);

    gAllocator *sysAlloc = gSystemAllocatorCreate();
    gTableTestInternal(sysAlloc);
    gAllocatorSelfFree(sysAlloc);
}

_component(position, {
    float x, y, z;
})

_component(velocity, {
    float x, y, z;
})

_component(foo, {
    int i;
})

void worldTest() {
    gWorld *w = gWorldCreate();


    _useWorld(w);

    _addComponentToWorld(position);
    _addComponentToWorld(velocity);

    //Create entity
    _createEntity(me0, _componentId(position), _componentId(velocity));
    _createEntity(me1, _componentId(position), _componentId(velocity));
    _createEntity(me2, _componentId(position), _componentId(velocity));
    _createEntity(se0, _componentId(position));

    testInt((int)w->archetypes.count, 2);

    {
        _useEntity(se0);
        testBool(_has(velocity), false);
        testBool(_has(position), true);
    }

    {
        _useEntity(me0);
        testBool(_has(velocity), true);
        testBool(_has(position), true);
    }

    {
        _useEntity(me0);
        position *pos0 = _take(position);
        testFloat(pos0->x, 0.0f);
        testFloat(pos0->y, 0.0f);
        testFloat(pos0->z, 0.0f);

        _reuseEntity(me1)
        position *pos1 = _take(position);
        pos1->x += 42;
        testFloat(pos1->x, 42.0f);
        testFloat(pos1->y, 0.0f);
        testFloat(pos1->z, 0.0f);
    }


    testBool(gWorldIsEntityAlive(w, me2), true);
    const gEntity invalidEntity = {0};
    testBool(gWorldIsEntityAlive(w, invalidEntity), false);

    gWorldDestroyEntity(w, me2);
    testBool(gWorldIsEntityAlive(w, me2), false);

    //test reuse entity and inc version
    _createEntity(me3, _componentId(position), _componentId(velocity));
    testInt(me3.archetype, me2.archetype);
    testInt(me3.index, me2.index);
    testInt(me3.version, me2.version + 1);
    testBool(gWorldIsEntityAlive(w, me2), false);

    int entityCount = 0;
    {
        _makeQuery
            _with(position)
            _with(velocity)
        _foreach {
            bool hasPosition = _has(position);
            testBool(hasPosition, true);
            position *p = _take(position);
            entityCount++;
        }
    }
    testInt(entityCount, 3);

    entityCount = 0;
    {
        _makeQuery
            _with(position)
        _foreach {
            entityCount++;
        }
    }
    testInt(entityCount, 4);

    entityCount = 0;
    {
        _makeQuery
            _without(velocity)
        _foreach {
            entityCount++;
        }
    }
    testInt(entityCount, 1);

    entityCount = 0;
    {
        _makeQuery
            _withAny(velocity)
            _withAny(position)
        _foreach {
            entityCount++;
        }
    }
    testInt(entityCount, 4);

    gWorldFree(w);
}

_tagComponent(tagComponent);

void xTest() {
    gWorld *w = gWorldCreate();

    _useWorld(w);

    _addComponentToWorld(position);
    _addComponentToWorld(velocity);
    _addComponentToWorld(tagComponent);

    int positionId = _componentId(position);
    int velocityId = _componentId(velocity);
    int tagComponentId = _componentId(tagComponent);

    testInt(positionId, 0);
    testInt(velocityId, 1);
    testInt(tagComponentId, 2);

    _createEntity(me0, _componentId(position), _componentId(velocity), _componentId(tagComponent));
    _createEntity(me1, _componentId(position), _componentId(velocity), _componentId(tagComponent));
    _createEntity(me2, _componentId(position), _componentId(velocity), _componentId(tagComponent));

    testInt(me0.index, 0);
    testInt(me1.index, 1);
    testInt(me2.index, 2);
    testInt(me0.archetype, 0);
    testInt(me1.archetype, 0);
    testInt(me2.archetype, 0);
    testInt(me0.version, 1);
    testInt(me1.version, 1);
    testInt(me2.version, 1);

    _createEntity(se0, _componentId(position), _componentId(tagComponent));
    testInt(se0.index, 0);
    testInt(se0.archetype, 1);
    testInt(se0.version, 1);

    _createEntity(ve0, _componentId(velocity), _componentId(tagComponent));
    _createEntity(ve1, _componentId(velocity), _componentId(tagComponent));
    _createEntity(ve2, _componentId(velocity), _componentId(tagComponent));
    testInt(ve0.index, 0);
    testInt(ve1.index, 1);
    testInt(ve2.index, 2);
    testInt(ve0.archetype, 2);
    testInt(ve1.archetype, 2);
    testInt(ve2.archetype, 2);
    testInt(ve0.version, 1);
    testInt(ve1.version, 1);
    testInt(ve2.version, 1);

    int iterCount = 0;
    {
        _makeQuery
            _with(tagComponent)
        _foreach {
            printf("e i: %d a: %d v: %d\n", __entity.index, __entity.archetype, __entity.version);

            bool hasVelocity = _has(velocity);
            if (!hasVelocity) {
                gWorldDestroyEntity(w, __entity);
            }
            iterCount++;
        }
    }

    testInt(iterCount, 7);


    gWorldFree(w);
}


int testIntHashFunction(const void *key, const size_t keySize) {
    return *(int*)key;
}

bool testIntEqualFunction(const void *key1, const void *key2, const size_t keySize) {
    return *(int*)key1 == *(int*)key2;
}

void hashMapTest() {

    gAllocator *alloc = gAllocatorCreate(16000, 32);

    gHashMap *hm = gHashMapCreate(
        alloc,
        sizeof(int),
        sizeof(int),
        &testIntHashFunction,
        &testIntEqualFunction,
        4);

    testBool(gHashMapAdd(hm, &(int){42},   &(int){69}), true);
    testBool(gHashMapAdd(hm, &(int){420},  &(int){42}), true);
    testBool(gHashMapAdd(hm, &(int){6969}, &(int){0}),  true);
    testBool(gHashMapAdd(hm, &(int){6969}, &(int){0}),  false);
    testInt((int)hm->count, 3);

    const int *v0 = gHashMapAt(hm, &(int){42});
    testInt(*v0, 69);
    const int *v1 = gHashMapAt(hm, &(int){420});
    testInt(*v1, 42);
    const int *v2 = gHashMapAt(hm, &(int){6969});
    testInt(*v2, 0);

    testBool(gHashMapContains(hm, &(int){42}),   true);
    testBool(gHashMapContains(hm, &(int){1934}), false);
    testBool(gHashMapRemove  (hm, &(int){42}),   true);
    testBool(gHashMapContains(hm, &(int){42}),   false);

    //resize hashmap test
    for (int i = 0; i < 16; i++) {
        testBool(gHashMapAdd(hm, &i, &i), true);
    }
    for (int i = 0; i < 16; i++) {
        const int *v = gHashMapAt(hm, &i);
        testInt(*v, i);
    }
    testBool(gHashMapContains(hm, &(int){420}),   true);
    testBool(gHashMapContains(hm, &(int){6969}),   true);

    gHashMapFree(hm);
    gAllocatorSelfFree(alloc);
}

void queryCacheTest() {
    gWorld *w = gWorldCreate();

    _useWorld(w);

    _addComponentToWorld(position);
    _addComponentToWorld(velocity);
    _addComponentToWorld(tagComponent);
    _addComponentToWorld(foo);

    _createEntity(me0, _componentId(position));

    {
        int count = 0;
        _makeQuery
            _with(position)
        _foreach {
            count++;
        }
        testInt(count, 1);
        testInt((int)w->queryCache.data->count, 1);
        const gQueryCacheEntry *e = gHashMapAt(w->queryCache.data, &__q);
        testBool(e != NULL, true);
        testInt(e->archetypesCount, 1);
    }

    _createEntity(me1, _componentId(position), _componentId(velocity));

    {
        int count = 0;
        _makeQuery
            _with(position)
        _foreach {
            count++;
        }
        testInt(count, 2);
        testInt((int)w->queryCache.data->count, 1);
        const gQueryCacheEntry *e = gHashMapAt(w->queryCache.data, &__q);
        testBool(e != NULL, true);
        testInt(e->archetypesCount, 2);

    }

    {
        int count = 0;
        _makeQuery
            _with(position)
        _foreach {
            if (count == 0) {
                _createEntity(me2, _componentId(position), _componentId(velocity), _componentId(tagComponent));
            }
            count++;
        }
        testInt(count, 3);
        testInt((int)w->queryCache.data->count, 1);
        const gQueryCacheEntry *e = gHashMapAt(w->queryCache.data, &__q);
        testBool(e != NULL, true);
        testInt(e->archetypesCount, 3);
    }

    {
        int count = 0;
        _makeQuery
            _with(position)
            _with(velocity)
        _foreach {
            count++;
        }
        testInt(count, 2);
        testInt((int)w->queryCache.data->count, 2);
        const gQueryCacheEntry *e = gHashMapAt(w->queryCache.data, &__q);
        testBool(e != NULL, true);
        testInt(e->archetypesCount, 3);
    }

    {
        int count = 0;
        _makeQuery
            _with(foo)
            _with(velocity)
        _foreach {
            count++;
        }
        testInt(count, 0);
        testInt((int)w->queryCache.data->count, 3);
        const gQueryCacheEntry *e = gHashMapAt(w->queryCache.data, &__q);
        testBool(e != NULL, true);
        testInt(e->archetypesCount, 3);
    }
    gWorldFree(w);
}

int main() {
    runTestCase(allocatorTest);
    runTestCase(gBitSetTest);
    runTestCase(gSliceTest);
    runTestCase(gVectorTest);
    runTestCase(gChunkedListTest);
    runTestCase(gTableTest);
    runTestCase(worldTest);
    runTestCase(xTest);
    runTestCase(hashMapTest);
    runTestCase(queryCacheTest);
    printf("All test passed!");
    return 0;
}