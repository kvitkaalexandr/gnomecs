#ifndef GNOMECS_H
#define GNOMECS_H

/**
 * All-in-one header for GnomeCS
*/

// Allocator
#include "allocators/allocator.h"
#include "allocators/systemallocator.h"
#include "allocators/helpers.h"

// Archetype
#include "archetype/archetype.h"

// Collections
#include "collections/absolute/hashmap.h"
#include "collections/bitset.h"
#include "collections/table.h"
#include "collections/vector.h"
#include "collections/slice.h"
#include "collections/chunkedlist.h"

// Components
#include "components/components.h"
#include "components/componentsdb.h"

// Log
#include "log/log.h"

// Search
#include "search/query.h"
#include "search/querycache.h"
#include "search/iterator.h"

// World
#include "world/world.h"
#include "world/entity.h"

#endif //GNOMECS_H
