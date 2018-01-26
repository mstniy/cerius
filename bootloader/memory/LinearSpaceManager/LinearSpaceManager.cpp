#include "LinearSpaceManager.h"

#include "common/kout.h"

AvlTree<RangePair>  LinearSpaceManager::treeStart(AddressSpaceManager::avlHeap);
AslrTree LinearSpaceManager::treeLength(AddressSpaceManager::aslrHeap);

uint64_t LinearSpaceManager::allocAslr(uint64_t length, uint64_t align)
{
	kassert(length%4096 == 0);
	kassert(align %4096 == 0);
	MemoryReserveManager::lock();
	RangePair super;
	uint64_t start;
	if (treeLength.findRandomRange(length+align-1, start, super)==false) // Find a range larger than 'length', to make sure it stays long enough even after alignment.
		return 0;
	if (start%align)
		start += align- start%align;
	kassert(start >= super.y);
	kassert(start+length <= super.y+super.x);
    AddressSpaceManager::allocSubrangeFromSuperRange(treeStart, treeLength, RangePair(length, start), super);
    MemoryReserveManager::unlock();
    return start;
}
void LinearSpaceManager::free(uint64_t start, uint64_t length)
{
	kassert(start %4096 == 0);
	kassert(length%4096 == 0);
	MemoryReserveManager::lock();
	AddressSpaceManager::free(treeStart, treeLength, start, length);
	MemoryReserveManager::unlock();
}
