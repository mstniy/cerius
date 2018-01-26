#ifndef MEMORY_RESERVE_MANAGER
#define MEMORY_RESERVE_MANAGER

#include <stdint.h>
#include "kassert.h"
#include "../PrimitiveHeap/PrimitiveHeap.h"

class MemoryReserveManager
{
private:
	static PrimitiveHeap pHeap;
	// Since calling MemoryManager::alloc has an amount of memory overhead to it (in the form of additional page table entries), we allocate huge chunks of memory from the MemoryManager, and divide them into smaller chunks using a PrimitiveHeap.
	static constexpr int primitiveHeapChunkSize = 2*1024*1024; // 2mb, an arbitrary choice.
	static constexpr int unitSize = 8192; // How many bytes will be allocated with each call to alloc()
	static constexpr int minNumUnits = 8; // The act of renewing the reserve is guaranteed to require at most 8 units of reserve memory (6 for pager, 2 for avl trees of linear and physical space managers (1 for each)). If the number of available units in pHeap drop below this value, we may never be able to renew the reserve again.
	static constexpr int maxNumUnits = 500; // An arbitrary choice.
private:
	MemoryReserveManager() = delete;
	MemoryReserveManager(const MemoryReserveManager&) = delete;
	~MemoryReserveManager() = delete;
	static void lock(); // Locks the underlying PrimitiveHeap. Other modules call lock() when either of the address spaces are unstable, ie., in the middle of an operation, or during the initialization of the memory manager. The underlying PrimitiveHeap increases its lockCnt during replenish/trim operations, too, to avoid infinite loops.
	static void unlock();
    static void* alloc(uint64_t size, uint64_t align); // Allocates a unit from the reserve. 'size' and 'align' must equal unitSize
    static void free(void* lin, uint64_t size); // Returns a unit to the reserve. lin must have been allocated from MemoryReserveManager. size must be equal to unitSize.

    friend class MemoryManagerInitializer;
    friend class Pager;
    friend class PhysicalSpaceManager;
    friend class LinearSpaceManager;
    friend class AddressSpaceManager;
};

#endif
