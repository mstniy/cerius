#ifndef PAGER_H
#define PAGER_H

#include "../Reserve/MemoryReserveManager.h"
#include "kassert.h"

// In AMD64, each page table is an array of 512 pointers, each of which contain physical addresses and some other data.
// To make manipulating the page tree easier, Pager stores the corresponding linear addresses of each child-table, too.
// As a result, each table is 8kb, the first 4kb is of the format defined by AMD64, the second 4kb contains linear pointers to child tables.
// For page table entries that map a page (instead of pointing to another table), the corresponding entry in the second table doesn't matter.

struct PageTreeNode
{
    uint64_t phy[512];
    PageTreeNode* lin[512];
};

class PagePrefs // Use a class instead of enum
{
public:
	constexpr static uint64_t us  = 1ULL<<2;
	constexpr static uint64_t pcd = 1ULL<<4;
	constexpr static uint64_t xd  = 1ULL<<63;
};

class Pager
{
private:
	static PageTreeNode* pml4; // Holds the linear address of the PML4
private:
	Pager() = delete;
	~Pager() = delete;

    static void invlpg(uint64_t p);
    static void pagein(uint64_t startPhy, uint64_t startLin, uint64_t length, int height, PageTreeNode* curPS, uint64_t startLinCurPS, uint64_t pref);
    // If the PageTreeNode given by curPS parameter has no present children left after pageout process, this function free's curPS and return true.
    static bool pageout(uint64_t startLin, uint64_t length, int height, PageTreeNode* curPS, uint64_t startLinCurPS);
public:
	// Maps the physical range [startPhy, startPhy+length) to linear range [startLin, startLin+length)
	// Uses the 'preferences' in *prefs* to determine special bits of page-mapping paging structures (such as U/S, PCD or XD).
    static void pagein(uint64_t startPhy, uint64_t startLin, uint64_t length, uint64_t prefs=0);
    // Unmaps the linear range [startLin, startLin+length)
    // Note that this function merely unmaps the range. It doesn't free the linear or physical space allocated for it.
    static void pageout(void* startLin, uint64_t length);
    // Finds the physical address corresponding to the given linear address by walking the page tree.
    // Returns 0 if the address is not mapped.
    static uint64_t lintophy(void* _p);
    static void initialize(void* _pml4);

    static_assert(MemoryReserveManager::unitSize == 8192, "MemoryReserveManager::unitSize != 8192");
    static_assert(sizeof(PageTreeNode) == 8192, "sizeof(PageTreeNode) != 8192");
};

#endif
