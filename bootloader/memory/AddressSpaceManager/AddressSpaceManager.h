#ifndef ADDRESS_SPACE_MANAGER_H
#define ADDRESS_SPACE_MANAGER_H

#include "../AvlTree/AvlTree.h"
#include "../AslrTree/AslrTree.h"
#include "../PrimitiveHeap/PrimitiveHeap.h"
#include "../Reserve/MemoryReserveManager.h"

using RangeTree = AvlTree<RangePair>;

class AddressSpaceManager
{
private:
	static PrimitiveHeap avlHeap, aslrHeap; // PrimitiveHeap's used by LinearSpaceManager and PhysicalSpaceManager
	static constexpr int heapMaxNumUnits = 400;
private:
	AddressSpaceManager() = delete;
	~AddressSpaceManager() = delete;
	// Allocates the interval [sub.y, sub.y+sub.x) from the given trees.
	// 'super' describes the avl node which contains the interval to be allocated.
	static void allocSubrangeFromSuperRange(RangeTree& treeStart, RangeTree& treeLength, const RangePair& sub, const RangePair& super);
	// Allocates the interval [range.y, range.y+range.x) from the given trees.
	static void alloc(RangeTree& treeStart, RangeTree& treeLength, const RangePair& range);
private:
	// Both LinearSpaceManager and PhysicalSpaceManager use this logic of managing available memory chunks over two AvlTree's. To avoid code duplication, it lives in this class.
	static RangePair free(RangeTree& treeStart, RangeTree& treeLength, uint64_t start, uint64_t length);

	friend class PhysicalSpaceManager; // This class merely helps these managers handle avl tree additions, deletions, range merging etc.
	friend class LinearSpaceManager;   // It is not meant to be used by any other class.
	friend class MemoryManagerInitializer;
	friend class Heap;
};

#endif
