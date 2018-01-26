#ifndef LINEAR_SPACE_MANAGER_H
#define LINEAR_SPACE_MANAGER_H

#include "../AddressSpaceManager/AddressSpaceManager.h"
#include "../Reserve/MemoryReserveManager.h"

class LinearSpaceManager
{
private:
	// Both of the tree's below keep free ranges.
	static AvlTree<RangePair>  treeStart; // Used to figure out whether a free'd range can be merged with another range adjacent to it.
	static AslrTree treeLength; // Used to figure out where a range at least as long as the requested length can be found.
	LinearSpaceManager() = delete;
	~LinearSpaceManager() = delete;
public:
    static uint64_t allocAslr(uint64_t length, uint64_t align=4096); // Allocates a random range of length 'length', aligned according to 'align'. Both parameters must be integer multiplies of 4096. Returns NULL for error.
	static void free(uint64_t start, uint64_t length); // Free's the given range.

	friend class MemoryManagerInitializer;
};

#endif
