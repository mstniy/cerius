#include "utils.h"

void memory_copy(BYTE* dest, BYTE* src, int size)
{
	while (size > 0)
	{
		(*(dest++))=(*(src++));
		size--;
	}
}

uint64_t divru(uint64_t x, uint64_t y)
{
	return (x+y-1)/y;
}

uint64_t rdtsc()
{
	uint32_t low, high;
	asm volatile("rdtsc":"=a"(low), "=d"(high));
	return ((uint64_t)high << 32) + low;
}
