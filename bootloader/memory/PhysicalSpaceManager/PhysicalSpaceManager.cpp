#include "PhysicalSpaceManager.h"
#include "common/utils.h"
#include "common/kout.h"

RangeTree PhysicalSpaceManager::treeStart(AddressSpaceManager::avlHeap);
RangeTree PhysicalSpaceManager::treeLength(AddressSpaceManager::avlHeap);
uint64_t  PhysicalSpaceManager::availableMemory;

uint64_t PhysicalSpaceManager::alloc(uint64_t length)
{
	kassert(length % 4096 == 0);
	MemoryReserveManager::lock();
	RangePair super(length, 0);
	if (treeLength.findBound(super, false)==false)
	{
		//kout << "-PSM::alloc: no range found\n";
		return 0;
	}
	kassert(super.x >= length);
    AddressSpaceManager::allocSubrangeFromSuperRange(treeStart, treeLength, RangePair(length, super.y), super);
    availableMemory -= length;
    MemoryReserveManager::unlock();
    return super.y;
}
void PhysicalSpaceManager::free(uint64_t start, uint64_t length)
{
	kassert(start %4096 == 0);
	kassert(length%4096 == 0);
	MemoryReserveManager::lock();
	AddressSpaceManager::free(treeStart, treeLength, start, length);
	availableMemory += length;
	MemoryReserveManager::unlock();
}
uint64_t PhysicalSpaceManager::getAvailableMemory()
{
	return availableMemory;
}
