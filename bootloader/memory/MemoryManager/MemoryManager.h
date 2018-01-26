#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "../Reserve/MemoryReserveManager.h"
#include "../LinearSpaceManager/LinearSpaceManager.h"
#include "../PhysicalSpaceManager/PhysicalSpaceManager.h"
#include "../Pager/Pager.h"

static_assert(sizeof(void*)==8, "This is an AMD64 memory manager.");

// This is the ultimate memory allocation class.
class MemoryManager
{
private:
	MemoryManager() = delete;
	~MemoryManager() = delete;
public:
	// Allocates a range of length 'length' from both the physical space and the linear space, carries out the mapping. Returns NULL for error.
    static void* alloc(size_t length, size_t align=4096);
    // Unmaps the given range from the linear space, frees the range in linear space, free's the underlying physical range.
    static void free(void* start, size_t length);
};

#endif
