#include "world/entity.h"
#include <stdlib.h>

bool gEntityEq(gEntity e1, gEntity e2)
{
    return e1.index == e2.index && e1.version == e2.version && e1.archetype == e2.archetype;
}