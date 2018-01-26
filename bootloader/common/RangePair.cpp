#include "RangePair.h"

RangePair::RangePair(uint64_t _x, uint64_t _y):
	x(_x),
	y(_y)
{
}

bool RangePair::operator<(const RangePair& o) const
{
	if (x != o.x)
		return x < o.x;
	return y < o.y;
}

bool RangePair::operator<=(const RangePair& o) const
{
	if (x != o.x)
		return x < o.x;
	return y <= o.y;
}

bool RangePair::operator==(const RangePair& o) const
{
	return (x == o.x) && (y == o.y);
}
