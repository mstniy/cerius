#include "common/kout.h"
#include "drivers/ps2kbd.h"
#include "drivers/Ata/Ide.h"
#include "drivers/Ata/Ahci.h"
#include "drivers/pci.h"
#include "drivers/mbr.h"
#include "memory/Initializer/MemoryManagerInitializer.h"
#include "memory/MemoryManager/MemoryManager.h"
#include "memory/Heap/Heap.h"
#include "asm/movestack.h"
#include "interrupt/interrupt.h"
#include "tests/Tests.h"

void print_smap_info(SmapEntry* smap, QWORD smap_length)
{
	kout << "\nThere are " << smap_length << " smap entries.\n";
	kout << "SMAP base address: " << smap << "\n";
	for (QWORD i=0;i<smap_length;i++)
	{
		kout << "Base: " << (void*)smap[i].base << " ";
		kout << "Size: " << smap[i].size << "\n";
	}
}

void PciTest()
{
	PciDevice arr[100];
	int n = PciDevice::EnumerateDevices(arr, 100);
	for (int i=0;i<n;i++)
	{
        kout << "dev" << i << ": bclass: " << arr[i].GetBaseClass();
        kout << " sclass: " << arr[i].GetSubClass() << "\n";
	}
}

extern "C" // Since this function is going to be linked with the assembly code in bootloader.asm, it must not be name-mangled.
void __attribute((sysv_abi)) bootloader_64bit_stage(void* pml4, SmapEntry* smap, QWORD smap_length, void* initialReserveStart, uint64_t initialReserveLength)
{
	clear_screen();
	kout << "Hello from the 64bit stage!\n";
	kout << (void*)0xDEADBEEF1BAD1DEA << "\n";
	kout << "Initial reserve is located at " << initialReserveStart << "\n";
	kout << "Initial reserve length: " << initialReserveLength << "\n";
	print_smap_info(smap, smap_length);
	kout << "Initializing memory manager...\n";
	MemoryManagerInitializer::Initialize(pml4, smap, smap_length, initialReserveStart, initialReserveLength);
	kout << "Moving stack...\n";
	const uint64_t stackMoveSize = sizeof(uint64_t)*897; // The multiplier needs to be an odd number to preserve 16-byte stack alignment, required by SSE2 instructions used by GCC.
	const uint64_t newStackSize = 2ULL*1024*1024; // 2mb
	void* newStack = MemoryManager::alloc(newStackSize);
	// MoveStack must be directly called from bootloader_64bit_stage, since it can't modify rbp values that exist on the stack.
	MoveStack((void*)((uint64_t)newStack+newStackSize-stackMoveSize), stackMoveSize);
	kout << "Paging out low memory...\n";
	Pager::pageout(0, 4096); // Page out low memory to get exceptions on null-pointer dereferences.
	kout << "Setting interrupt handlers...\n";
	SetISRs();
	//asm volatile("xchg %bx, %bx\n\tint $89\n\txchg %bx, %bx");
	//kout<<*(uint64_t*)16;
	kout << "Initializing heap...\n";;
	Heap::Init();
	/*kout << "Starting memory test...\n";
	for (int i=0;i<5;i++) MemoryStressTest(i);
	kout << "Starting heap stress test...\n";
	for (int i=0;i<5;i++) HeapStressTest(i);
	kout << "Starting heap types test...\n";
	for (int i=0;i<5;i++) HeapTypesTest(i);*/
	//PciTest();
	kout << "Starting storage test...\n";
	StorageTest();
	kout << "Hanging...";
	asm volatile("cli\n\thlt");
}
