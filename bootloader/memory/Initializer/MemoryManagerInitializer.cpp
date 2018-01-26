#include "MemoryManagerInitializer.h"

#include "common/kout.h"

MemoryManagerInitializer::MMIRange::MMIRange(uint64_t _s, uint64_t _e):
	s(_s),
	e(_e)
{}

void MemoryManagerInitializer::PartitionRange(const MMIRange& range, const MMIRange* forbids, int lenForbids, void(*callback)(const MMIRange&), int i)
{
	if (i == lenForbids)
		callback(range);
	if (range.s<forbids[i].s)
		PartitionRange(MMIRange(range.s,std::min(range.e, forbids[i].s)),forbids, lenForbids, callback, i+1);
	if (range.e>forbids[i].e)
		PartitionRange(MMIRange(std::max(range.s, forbids[i].e), range.e),forbids, lenForbids, callback, i+1);
}

void MemoryManagerInitializer::Initialize(void* pml4, SmapEntry* smap, int smap_length, void* initialReserveStart, size_t initialReserveLength)
{
	const uint64_t oneMB = 1048576;

	kassert((uint64_t)initialReserveStart % MemoryReserveManager::primitiveHeapChunkSize == 0);
	kassert(initialReserveLength == MemoryReserveManager::primitiveHeapChunkSize);

	kout << "Initializing static fields\n";
	new (&AddressSpaceManager::avlHeap) PrimitiveHeap(8192, sizeof(AvlEntry<RangePair>), 8, MemoryReserveManager::alloc, MemoryReserveManager::free, NULL, 0, AddressSpaceManager::heapMaxNumUnits);
	new (&AddressSpaceManager::aslrHeap) PrimitiveHeap(8192, sizeof(AslrEntry), 8, MemoryReserveManager::alloc, MemoryReserveManager::free, NULL, 0, AddressSpaceManager::heapMaxNumUnits);
	new (&PhysicalSpaceManager::treeStart) RangeTree(AddressSpaceManager::avlHeap);
	new (&PhysicalSpaceManager::treeLength) RangeTree(AddressSpaceManager::avlHeap);
	new (&LinearSpaceManager::treeStart) RangeTree(AddressSpaceManager::avlHeap);
	new (&LinearSpaceManager::treeLength) AslrTree(AddressSpaceManager::aslrHeap);
	new (&MemoryReserveManager::pHeap) PrimitiveHeap(MemoryReserveManager::primitiveHeapChunkSize, MemoryReserveManager::unitSize, MemoryReserveManager::unitSize, MemoryManager::alloc, MemoryManager::free, initialReserveStart, MemoryReserveManager::minNumUnits, MemoryReserveManager::maxNumUnits);

	MemoryReserveManager::lock();

	kout << "Initializing the pager\n";
    Pager::initialize(pml4);

    uint64_t iResPhy = Pager::lintophy(initialReserveStart); // Pager::lintophy allocates no memory
    kout << "Physical address of the initialReserve is " << (void*)iResPhy << "\n";

	kout << "Initializing the PSM\n";
	const MMIRange psmForbids[]={ // TODO: Ideally, we should also not free a range if it contains any of the paging structures currently in use.
								  // But the current 16-bit stage stores all the paging structures it creates in the first megabyte, so this is not a pressing issue.
		{0, oneMB},
		{iResPhy, iResPhy+initialReserveLength}
	};
    for (int i=0;i<smap_length;i++)
	{
        uint64_t base = smap[i].base;
        uint64_t  end = smap[i].base + smap[i].size;
		if (base % 4096)
			base += base - base%4096;
		end -= end%4096;
		if (base >= end)
			continue;
		PartitionRange(MMIRange(base, end), psmForbids, sizeof(psmForbids)/sizeof(MMIRange), [](const MMIRange& range){
			kassert(range.e > range.s);
			PhysicalSpaceManager::free(range.s, range.e-range.s);
		});
	}

	kout << "Initializing the LSM\n";
	const MMIRange lsmForbids[]={
		{0, oneMB},
		{(uint64_t)initialReserveStart, (uint64_t)initialReserveStart+initialReserveLength}
	};
	PartitionRange(MMIRange(0, 1ULL<<47), lsmForbids, sizeof(lsmForbids)/sizeof(MMIRange), [](const MMIRange& range){ // 1ULL<<47 is the length of AMD64 linear space.
		kassert(range.e > range.s);
		LinearSpaceManager::free(range.s, range.e-range.s);
	});

    MemoryReserveManager::unlock(); // Spin things up
}
