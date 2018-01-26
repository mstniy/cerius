#include "BitmapHelper.h"

uint64_t leastSignificantSetBitOf(uint64_t x)
{
	//return x&((~x)+1);
	return x&(-x); // Assumes two's complement.
}

bool isPowerOfTwo(uint64_t x)
{
	kassert(x != 0);
	return x == leastSignificantSetBitOf(x);
}

uint64_t binarylog(uint64_t x)
{
	kassert(isPowerOfTwo(x));

	static const int bytelog[]= {0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7};
	int res=0;
	if ((x & 0xFFFFFFFF)==0)
	{
		res += 32;
		x >>=32;
	}
	if ((x & 0xFFFF)==0)
	{
		res += 16;
		x >>= 16;
	}
	if ((x & 0xFF)==0)
	{
		res += 8;
		x >>= 8;
	}
	return bytelog[x]+res;
}
