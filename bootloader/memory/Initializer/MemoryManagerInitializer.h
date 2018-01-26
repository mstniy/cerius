#ifndef MEMORY_MANAGER_INITIALIZER_H
#define MEMORY_MANAGER_INITIALIZER_H

#include <algorithm>
#include "../PhysicalSpaceManager/PhysicalSpaceManager.h"
#include "../LinearSpaceManager/LinearSpaceManager.h"
#include "../Pager/Pager.h"
#include "../Reserve/MemoryReserveManager.h"
#include "../MemoryManager/MemoryManager.h"

struct SmapEntry
{
	uint64_t base;
	uint64_t size;
};

class MemoryManagerInitializer
{
private:
	struct MMIRange
	{
		uint64_t s, e;
		MMIRange(uint64_t _s, uint64_t _e);
	};
	// Finds the difference of the given range *range* to the list *forbids*.
	// Since the result may contain many individual non-overlapping ranges, calls *callback* for each part in the result.
	static void PartitionRange(const MMIRange& range, const MMIRange* forbids, int lenForbids, void(*callback)(const MMIRange&), int i=0);
public:
	MemoryManagerInitializer() = delete;
	~MemoryManagerInitializer() = delete;
	// Initializes the entire memory manager (not including the Heap) with the given data.
	// pml4 describes where pml4 is mapped in *linear* space. Note that the pml4 must be mapped to linear memory before the memory manager can be initialized.
	// smap and smap_length describes where usable memory lies in the physical address space, presumably returned from BIOS. Note that the array itself must be mapped in linear space.
	// initialReserveStart and initialReserveLength describes where the initial reserve lies in *linear* space. Note that the initial reserve must be mapped to linear memory before the memory manager can be initialized.
	// Since the initial reserve will be given to MemoryReserveManager::pHeap, initialReserveStart must be MemoryReserveManager::primitiveHeapChunkSize-aligned, and initialReserveLength must equal MemoryReserveManager::primitiveHeapChunkSize.
	// The page table format used by the Pager is slightly different than that of AMD64. The page table must conform to this format before the memory manager can be initialized. For details, see Pager.
    static void Initialize(void* pml4, SmapEntry* smap, int smap_length, void* initialReserveStart, size_t initialReserveLength);
};

#endif
