#ifndef RANGE_PAIR_H
#define RANGE_PAIR_H

#include <stdint.h>

// Keeps an x,y pair. One describes where the range starts, the other describes how long the range is (in bytes), but not respectively.
// To have both fast range searches and fast range merges, address space managers (PhysicalSpaceManager and LinearSpaceManager) need two AvlTree's, one sorted by starting address, one sorted by length.
// *x* has precedence for inequality.
class RangePair
{
public:
	uint64_t x, y;
	RangePair() = default;
	RangePair(uint64_t _x, uint64_t _y);
	bool operator<(const RangePair& o) const;
	bool operator<=(const RangePair& o) const;
	bool operator==(const RangePair& o) const;
};

#endif
