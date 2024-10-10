#include "search/query.h"

int gQueryGetHashCode(const gQuery query) {
    int hash = 0;

    hash = 31 * hash + gBitSetGetHashCode(query.all);
    hash = 31 * hash + gBitSetGetHashCode(query.any);
    hash = 31 * hash + gBitSetGetHashCode(query.none);

    return hash;
}

bool gQueryEquals(const gQuery query, const gQuery other) {
    return gBitSetEq(&query.all, &other.all) &&
           gBitSetEq(&query.any, &other.any) &&
           gBitSetEq(&query.none, &other.none);
}