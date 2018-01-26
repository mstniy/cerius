#include "kassert.h"
#include "type.h"
#include "kout.h"
#include "memory/MemoryManager/MemoryManager.h"

void MemoryStressTest(int cycle)
{
	const int n=512;
	kout << "Memory test cycle " << cycle << ".\n";
	BYTE** ptrs = (BYTE**)MemoryManager::alloc(n*sizeof(BYTE*));
	kout << "Allocating... ";
	for (int i=0;i<n;i++)
		ptrs[i] = (BYTE*)MemoryManager::alloc(4096);
	kout << "Writing... ";
	for (int i=0;i<n;i++)
	{
		ptrs[i][0] = cycle*i;
		ptrs[i][1] = cycle*i+5;
		ptrs[i][4095] = cycle*3*i;
	}
	kout << "Reading... ";
	for (int i=0;i<n;i++)
	{
		const BYTE sum = ptrs[i][0]+ptrs[i][1]+ptrs[i][4095];
		if (sum != BYTE(cycle*5*i+5))
		{
			kout << "Values evaporated.\n";
			kout << "i: " << i << "\n";
			kout << "Expected " << BYTE(cycle*5*i+5) << ", found " << sum << "\n";
			kassert(0);
		}
	}
	kout << "Freeing... ";
	for (int i=0;i<n;i++)
		MemoryManager::free(ptrs[i], 4096);
	MemoryManager::free(ptrs, n*sizeof(BYTE*));
	kout << "Done! (" << PhysicalSpaceManager::getAvailableMemory() << " bytes free)\n";
}
