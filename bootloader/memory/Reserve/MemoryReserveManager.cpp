#include "MemoryReserveManager.h"
#include "../MemoryManager/MemoryManager.h"

#include "common/kout.h"

PrimitiveHeap MemoryReserveManager::pHeap(primitiveHeapChunkSize, unitSize, unitSize, MemoryManager::alloc, MemoryManager::free, NULL, MemoryReserveManager::minNumUnits, MemoryReserveManager::maxNumUnits);

void* MemoryReserveManager::alloc(uint64_t size, uint64_t align)
{
	return pHeap.alloc(unitSize, unitSize);
}

void MemoryReserveManager::free(void* lin, uint64_t size)
{
	pHeap.free(lin, unitSize);
}

void MemoryReserveManager::lock()
{
	pHeap.lock();
}

void MemoryReserveManager::unlock()
{
	pHeap.unlock();
}
