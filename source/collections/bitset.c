#include "collections/bitset.h"

#include <stdarg.h>
#include <sys/_types/_size_t.h>

void gBitSetSet(gBitSet* bitSet, const unsigned int bit) {
    const size_t maskIndex = bit / gBitSetBitsPerLong;
    const size_t bitIndex = bit % gBitSetBitsPerLong;
    bitSet->bits[maskIndex] |= 1UL << bitIndex;
}

void gBitSetSetN(gBitSet* bitSet, const unsigned int n, ...) {
    va_list args;
    va_start(args, n);
    for (size_t i = 0; i < n; i++) {
        unsigned int bit = va_arg(args, unsigned int);
        gBitSetSet(bitSet, bit);
    }
    va_end(args);
}

void gBitSetClear(gBitSet* bitSet, const unsigned int bit) {
    const size_t maskIndex = bit / gBitSetBitsPerLong;
    const size_t bitIndex = bit % gBitSetBitsPerLong;
    bitSet->bits[maskIndex] &= ~(1UL << bitIndex);
}

bool gBitSetIsSet(const gBitSet* bitSet, const unsigned int bit) {
    const size_t maskIndex = bit / gBitSetBitsPerLong;
    const size_t bitIndex = bit % gBitSetBitsPerLong;
    unsigned long result = bitSet->bits[maskIndex] & (1UL << bitIndex);
    return result != 0UL;
}

bool gBitSetAll(const gBitSet* bitSet, const gBitSet* otherBitSet) {
    const unsigned long* bits = bitSet->bits;
    const unsigned long* oBits = otherBitSet->bits;
    for (unsigned long i = 0; i < gBitSetSize; i++) {
        if ((bits[i] & oBits[i]) != oBits[i]) return false;
    }
    return true;
}

bool gBitSetAny(const gBitSet* bitSet, const gBitSet* otherBitSet) {
    const unsigned long* bits = bitSet->bits;
    const unsigned long* oBits = otherBitSet->bits;
    for (unsigned long i = 0; i < gBitSetSize; i++) {
        if ((bits[i] & oBits[i]) != 0UL) return true;
    }
    return false;
}

bool gBitSetNone(const gBitSet* bitSet, const gBitSet* otherBitSet) {
    const unsigned long* bits = bitSet->bits;
    const unsigned long* oBits = otherBitSet->bits;
    for (unsigned long i = 0; i < gBitSetSize; i++) {
        if ((~bits[i] & oBits[i]) != oBits[i]) return false;
    }
    return true;
}

bool gBitSetEq(const gBitSet* bitSet, const gBitSet* otherBitSet) {
    const unsigned long* bits = bitSet->bits;
    const unsigned long* oBits = otherBitSet->bits;
    for (unsigned long i = 0; i < gBitSetSize; i++) {
        if (bits[i] != oBits[i]) return false;
    }
    return true;
}

bool gBitSetEmpty(const gBitSet* bitSet) {
    const unsigned long* bits = bitSet->bits;
    for (unsigned long i = 0; i < gBitSetSize; i++) {
        if (bits[i] != 0UL) return false;
    }
    return true;
}

unsigned int gBitSetCount(const gBitSet* bitSet) {
    unsigned int count = 0;
    for (unsigned long i = 0, max = gBitSetSize * gBitSetBitsPerLong; i < max; i++) {
        if (gBitSetIsSet(bitSet, i)) count++;
    }
    return count;
}
