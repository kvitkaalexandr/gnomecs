#include "collections/absolute/hashmap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

//todo: rewrite this to use relative pointers

size_t getBucket(gHashMap *map, const void *key) {
    return map->hashFunction(key, map->keySize) % map->capacity;
}

gHashMap *gHashMapCreate(size_t keySize, size_t valueSize, gHashFunction hashFunction, gEqualFunction equalFunction, size_t capacity) {
    gHashMap *map = malloc(sizeof(gHashMap));
    if (map == NULL) return NULL;

    map->keySize = keySize;
    map->valueSize = valueSize;
    map->capacity = capacity;
    map->count = 0;
    map->hashFunction = hashFunction;
    map->equalFunction = equalFunction;
    map->buckets = malloc(sizeof(gHashMapEntry) * capacity);
    memset(map->buckets, 0, sizeof(gHashMapEntry) * capacity);

    return map;
}

void gHashMapFree(gHashMap *map) {
    assert(map != NULL);
    if (map->buckets != NULL) {
        for (size_t i = 0; i < map->capacity; i++) {
            gHashMapEntry *entry = map->buckets[i];
            while (entry != NULL) {
                gHashMapEntry *next = entry->next;
                free(entry->key);
                free(entry->value);
                free(entry);
                entry = next;
            }
        }
    }
    free(map);
}

gHashMapEntry *lookup(const gHashMap *map, const int bucket, const void *key) {
    gHashMapEntry *entry = map->buckets[bucket];
    if (entry == NULL) return NULL;

    do {
        if (map->equalFunction(entry->key, key, map->keySize)) {
            return entry;
        }
        entry = entry->next;
    } while (entry != NULL);

    return NULL;
}

gHashMapEntry *addNewEntry(const gHashMap *map, const int bucket, const void *key) {
    gHashMapEntry *newEntry = malloc(sizeof(gHashMapEntry));
    if (newEntry == NULL) return NULL;
    newEntry->key = malloc(map->keySize);
    if (newEntry->key == NULL) {
        free(newEntry);
        return NULL;
    }
    newEntry->value = malloc(map->valueSize);
    if (newEntry->value == NULL) {
        free(newEntry->key);
        free(newEntry);
        return NULL;
    }
    gHashMapEntry *head = map->buckets[bucket];
    newEntry->next = head;
    map->buckets[bucket] = newEntry;
    return map->buckets[bucket];
}

void resizeHashMap(gHashMap *map) {
    const size_t oldCapacity = map->capacity;
    const size_t newCapacity = oldCapacity * 2;
    gHashMapEntry **oldBuckets = map->buckets;

    map->buckets = malloc(sizeof(gHashMapEntry) * newCapacity);
    if (map->buckets == NULL) {
        map->buckets = oldBuckets;
        return;
    }
    memset(map->buckets, 0, sizeof(gHashMapEntry) * newCapacity);
    map->capacity = newCapacity;

    for (size_t i = 0; i < oldCapacity; i++) {
        gHashMapEntry *entry = oldBuckets[i];
        while (entry != NULL) {
            gHashMapEntry *next = entry->next;
            const size_t newBucket = getBucket(map, entry->key);
            entry->next = map->buckets[newBucket];
            map->buckets[newBucket] = entry;
            entry = next;
        }
    }

    free(oldBuckets);
}

bool gHashMapAdd(gHashMap *map, const void *key, const void *value) {
    assert(map != NULL);

    if (map->count >= map->capacity) {
        resizeHashMap(map);
    }

    const int bucket = getBucket(map, key);

    const gHashMapEntry *e = lookup(map, bucket, key);
    if (e != NULL) return false;

    e = addNewEntry(map, bucket, key);
    if (e == NULL) return false;

    memcpy(e->key, key, map->keySize);
    memcpy(e->value, value, map->valueSize);
    map->count++;

    return true;
}

void *gHashMapAt(gHashMap *map, const void *key) {
    assert(map != NULL);
    const size_t bucket = getBucket(map, key);
    const gHashMapEntry *e = lookup(map, bucket, key);
    if (e == NULL) return NULL;
    return e->value;
}

bool gHashMapContains(gHashMap *map, const void *key) {
    assert(map != NULL);
    const size_t bucket = getBucket(map, key);
    return lookup(map, bucket, key) != NULL;
}

bool gHashMapRemove(gHashMap *map, const void *key) {
    assert(map != NULL);
    const size_t bucket = getBucket(map, key);
    gHashMapEntry *entry = map->buckets[bucket];
    if (entry == NULL) return false;

    if (map->equalFunction(entry->key, key, map->keySize)) {
        map->buckets[bucket] = entry->next;
        free(entry->key);
        free(entry->value);
        free(entry);
        map->count--;
        return true;
    }

    while (entry->next != NULL) {
        if (map->equalFunction(entry->next->key, key, map->keySize)) {
            gHashMapEntry *next = entry->next->next;
            free(entry->next->key);
            free(entry->next->value);
            free(entry->next);
            entry->next = next;
            map->count--;
            return true;
        }
        entry = entry->next;
    }

    return false;
}