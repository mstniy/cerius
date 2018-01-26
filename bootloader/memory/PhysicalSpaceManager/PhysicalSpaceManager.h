#ifndef PHYSICAL_SPACE_MANAGER_H
#define PHYSICAL_SPACE_MANAGER_H

#include "../AddressSpaceManager/AddressSpaceManager.h"
#include "../Reserve/MemoryReserveManager.h"

class PhysicalSpaceManager
{
private:
	static RangeTree treeStart, treeLength; // See the explanation of LinearSpaceManager::treeStart and treeLength
	static uint64_t availableMemory;
	PhysicalSpaceManager() = delete;
	~PhysicalSpaceManager() = delete;
public:
    static uint64_t alloc(uint64_t length);
	static void free(uint64_t start, uint64_t length);
	static uint64_t getAvailableMemory();

	friend class MemoryManagerInitializer;
};

#endif
