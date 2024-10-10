#ifndef BITSET_H
#define BITSET_H
#include <stdbool.h>

typedef unsigned long gBitSetElement;
#define gBitSetBitsPerLong (sizeof(gBitSetElement) * 8)
#define gBitSetSize 4

/**
 * Bitset of gBitSetBitsPerLong * gBitSetSize bits
 */
typedef struct {
    gBitSetElement bits[gBitSetSize];
} gBitSet;


/**
 * Set the bit at the specified index
 * @param bitSet Bitset
 * @param bit Bit to set
 */
void gBitSetSet(gBitSet* bitSet, const unsigned int bit);

/**
 * Set multiple bits
 * @param bitSet Bitset
 * @param n Number of bits
 * @param ... Bits to set
 */
void gBitSetSetN(gBitSet* bitSet, const unsigned int n, ...);

/**
 * Clear the bit at the specified index
 * @param bitSet Bitset
 * @param bit Bit to clear
 */
void gBitSetClear(gBitSet* bitSet, const unsigned int bit);

/**
 * Check if the bit at the specified index is set
 * @param bitSet Bitset
 * @param bit Bit to check
 * @return
 */
bool gBitSetIsSet(const gBitSet* bitSet, const unsigned int bit);

/**
 * Check if all bits in the bitset are set in the other bitset
 * @param bitSet Bitset
 * @param otherBitSet Other bitset
 * @return
 */
bool gBitSetAll(const gBitSet* bitSet, const gBitSet* otherBitSet);

/**
 * Check if any bit in the bitset is set in the other bitset
 * @param bitSet Bitset
 * @param otherBitSet Other bitset
 * @return
 */
bool gBitSetAny(const gBitSet* bitSet, const gBitSet* otherBitSet);

/**
 * Check if no bit in the bitset is set in the other bitset
 * @param bitSet Bitset
 * @param otherBitSet Other bitset
 * @return
 */
bool gBitSetNone(const gBitSet* bitSet, const gBitSet* otherBitSet);

/**
 * Check if the bitsets are equal
 * @param bitSet Bitset
 * @param otherBitSet Other bitset
 * @return
 */
bool gBitSetEq(const gBitSet* bitSet, const gBitSet* otherBitSet);

/**
 * Check if the bitset is empty
 * @param bitSet Bitset
 * @return
 */
bool gBitSetEmpty(const gBitSet* bitSet);

/**
 * Count the number of set bits in the bitset
 * @param bitSet Bitset
 * @return
 */
unsigned int gBitSetCount(const gBitSet* bitSet);

int gBitSetGetHashCode(const gBitSet bitSet);

#endif //BITSET_H
