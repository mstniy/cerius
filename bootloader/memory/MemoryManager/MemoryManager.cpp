#include "MemoryManager.h"

#include "common/kout.h"

void* MemoryManager::alloc(size_t length, size_t align)
{
	kassert(length % 4096 == 0);
	kassert(align  % 4096 == 0);
	uint64_t phy = PhysicalSpaceManager::alloc(length); // TODO: The physical address space may be fragmented, we must try allocating smaller chunks of contiguous physical memory if this call fails.
    if (phy == 0)
	{
    	return NULL;
	}
	kassert(phy%4096 == 0);
    uint64_t lin = LinearSpaceManager::allocAslr(length, align);
    kassert(lin%align == 0);
    if (lin == 0)
	{
		//kout << "Error : MemoryManager::alloc failed to allocate linear memory.\n";
        PhysicalSpaceManager::free(phy, length);
        return NULL;
	}
    Pager::pagein(phy, lin, length);
    return (void*)lin;
}

void MemoryManager::free(void* _start, size_t length)
{
	const uint64_t start = (uint64_t)_start;
	kassert(start % 4096 == 0);
	kassert(length % 4096 == 0);

    uint64_t phy = Pager::lintophy(_start);
    kassert(phy != 0);
    kassert(phy % 4096 == 0);
    Pager::pageout(_start, length); // It's important that we free the linear range after paging it out, because Pager::pageout may end up calling MemoryReserveManager::unlock(), which may try to replenish itself by allocating the very same region of linear memory that is being allocated by Pager::pageout (although this is very unlikely).
    PhysicalSpaceManager::free(phy, length); // TODO: The entire range may not be continuous on the physical memory. We may need to call PhysicalSpaceManager::free many times to free the entire physical range.
    LinearSpaceManager::free(start, length);
}
