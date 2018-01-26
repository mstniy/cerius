#ifndef BITMAP_HELPER_H
#define BITMAP_HELPER_H

#include <stdint.h>
#include "kassert.h"

uint64_t leastSignificantSetBitOf(uint64_t x);
bool isPowerOfTwo(uint64_t x);
uint64_t binarylog(uint64_t x);

#endif
