#include "AddressSpaceManager.h"

PrimitiveHeap AddressSpaceManager::avlHeap(8192, sizeof(AvlEntry<RangePair>), 8, MemoryReserveManager::alloc, MemoryReserveManager::free, NULL, 0, AddressSpaceManager::heapMaxNumUnits);
PrimitiveHeap AddressSpaceManager::aslrHeap(8192, sizeof(AslrEntry), 8, MemoryReserveManager::alloc, MemoryReserveManager::free, NULL, 0, AddressSpaceManager::heapMaxNumUnits);

void AddressSpaceManager::allocSubrangeFromSuperRange(RangeTree& treeStart, RangeTree& treeLength, const RangePair& sub, const RangePair& super)
{
	const uint64_t superEnd = super.y + super.x;
	const uint64_t   subEnd =   sub.y +   sub.x;
    treeLength.del(RangePair(super.x, super.y));
    treeStart.del(RangePair(super.y, super.x)); // For treeStart and treeLength, x and y are held in opposite order.
	if (super.y < sub.y)
	{
		treeLength.add(RangePair(sub.y-super.y, super.y));
		treeStart.add(RangePair(super.y, sub.y-super.y));
	}
	if (superEnd > subEnd)
	{
		treeLength.add(RangePair(superEnd-subEnd, subEnd));
		treeStart.add(RangePair(subEnd, superEnd-subEnd));
	}
}

void AddressSpaceManager::alloc(RangeTree& treeStart, RangeTree& treeLength, const RangePair& range)
{
    RangePair super(range.y, UINT64_MAX);
	kassert(treeStart.findBound(super, true));
    kassert(super.x+super.y >= range.y+range.x); // We shall only allocate ranges that are free
    allocSubrangeFromSuperRange(treeStart, treeLength, range, RangePair(super.y, super.x));
}

RangePair AddressSpaceManager::free(RangeTree& treeStart, RangeTree& treeLength, uint64_t start, uint64_t size)
{
	uint64_t mergedStart=start, mergedEnd=start+size;
    RangePair p(start, 0);
    if (treeStart.findBound(p, true)) // If there is an available range that ends just at the beginning of the given range, merge them.
	{
		kassert(p.x+p.y <= start); // We shall not free a range that is already free
		if (p.x+p.y == start)
		{
			mergedStart = p.x;
			treeStart.del(RangePair(p.x, p.y));
			treeLength.del(RangePair(p.y, p.x));
		}
	}
	p = RangePair(start, 0);
	if (treeStart.findBound(p, false)) // If there is an available range that begins just at the end of the given range, merge them.
	{
		kassert(p.x >= start+size); // We shall not free a range that is already free
		if (p.x == start+size)
		{
			mergedEnd = p.x+p.y;
			treeStart.del(RangePair(p.x, p.y));
			treeLength.del(RangePair(p.y, p.x));
		}
	}
	const uint64_t mergedSize = mergedEnd-mergedStart;
    treeStart.add(RangePair(mergedStart, mergedSize));
    treeLength.add(RangePair(mergedSize, mergedStart));
    return RangePair(mergedStart, mergedSize);
}
