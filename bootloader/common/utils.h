#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include "type.h"
#include <stdint.h>

#define BOCHS_BREAKPOINT  {asm volatile("xchg %bx, %bx");}

#ifndef UINT64_MAX
#define UINT64_MAX (~0ULL)
#endif

void memory_copy(BYTE* dest, BYTE* src, int size);

template<typename T>
T min(const T& t1, const T& t2)
{
	return (t1<t2)?t1:t2;
}

template<typename T> // There is another abs in cstdlib, so this abs is called 'myabs'
T myabs(const T& t)
{
	return (t<0)?-t:t;
}

uint64_t divru(uint64_t x, uint64_t y); // ceil(x/y)

uint64_t rdtsc();

#endif
