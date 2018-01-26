#include "Pager.h"

#include "common/kout.h"

PageTreeNode* Pager::pml4=0;

void Pager::invlpg(uint64_t p)
{
	asm volatile("invlpg (%[p])"
		:
		: [p] "r"(p)
	);
}
void Pager::pagein(uint64_t startPhy, uint64_t startLin, uint64_t length, int height, PageTreeNode* curPS, uint64_t startLinCurPS, uint64_t prefs)
{
	kassert(startPhy % 4096 == 0);
	kassert(startLin % 4096 == 0);
	kassert(length   % 4096 == 0);
	kassert(height >= 0 && height <= 3);
	const uint64_t childSize = 1ULL<<(12+height*9);
	kassert(startLinCurPS % childSize == 0);

	int indexStart = (startLin - startLinCurPS) / childSize;
	int indexEnd = divru((startLin+length - startLinCurPS), childSize);
	if (indexStart < 0)
		indexStart = 0;
	if (indexEnd > 512)
		indexEnd = 512;
	for (int i=indexStart; i<indexEnd; i++)
	{
		const uint64_t curChildLin = startLinCurPS + i*childSize;
		kassert(curChildLin<startLin+length);
		if (height > 0) // If this child needs to point to another table
		{
			if ((curPS->phy[i] & 1) == 0) // If this child is not present, create and initialize it.
			{
				MemoryReserveManager::lock(); // Lock the reserve here so that when we call alloc below, it doesn't try to renew itself and modify the page table in the prcoess.
				PageTreeNode* const newChildLin = (PageTreeNode*)MemoryReserveManager::alloc(8192, 8192);
				//kout << "pagein:Created child at lin addr " << newChildLin << "\n";
				const uint64_t newChildPhy = Pager::lintophy(newChildLin);
				kassert(newChildPhy != 0);
				curPS->lin[i] = newChildLin;
				curPS->phy[i] = newChildPhy | 3;
				for (int i=0;i<512;i++)
					newChildLin->phy[i] = 0;
				MemoryReserveManager::unlock();
			}
			pagein(startPhy, startLin, length, height-1, curPS->lin[i], curChildLin, prefs); // Recur to the child table
		}
		else // If this child needs to map a page
		{
			kassert(curChildLin>=startLin);
			kassert((curPS->phy[i] & 1) == 0); // Don't allow re-mapping a page
			const uint64_t offset = curChildLin - startLin;
			//TODO: For the paging structures that map a page instead of referencing another paging structure, we don't need the 'lin' field.
			curPS->phy[i] = (startPhy + offset) | 3ULL | prefs;
		}
	}
}
bool Pager::pageout(uint64_t startLin, uint64_t length, int height, PageTreeNode* curPS, uint64_t startLinCurPS)
{
	kassert(height >= 0 && height <= 3);
	const uint64_t childSize = ((1ULL<<(height*9))*4096);
	int indexStart = (startLin - startLinCurPS) / childSize;
	int indexStartMod = (startLin - startLinCurPS) % childSize;
	int indexEnd = divru((startLin+length - startLinCurPS), childSize);
	int indexEndMod = (startLin+length - startLinCurPS) % childSize;
	if (height == 0)
		kassert(indexStartMod == 0 && indexEndMod == 0);
	if (indexStart < 0)
		indexStart = indexStartMod = 0;
	if (indexEnd > 512)
	{
		indexEnd = 512;
		indexEndMod = 0;
	}
	//kout << "height: " << height << " indexStart: " << indexStart << " indexEnd: " << indexEnd << " childSize: " << childSize << "\n";
	for (int i=indexStart; i<indexEnd; i++)
	{
		kassert(curPS->phy[i] & 1); // Present
		if ((height == 0) || (curPS->phy[i] & (1ULL<<7))) // If the current child does map a page
		{
			if((i != indexStart   || indexStartMod == 0) && // If the current child is completely covered by the range given to us
			   (i != indexEnd-1   || indexEndMod   == 0))
			{
				curPS->phy[i] = 0;
				invlpg(startLinCurPS + i*childSize);
			}
			else // If we're freeing only a part of linear memory that was mapped in chunk using the page-size bit, things get tricky.
			{
				kassert(height > 0);
				kassert(height != 3); // Pagesize bit is not allowed for PML4 entries.
				const uint64_t curChildPhyRangeStart = curPS->phy[i] & ~0xFFF00000001FFFFFULL;
				const uint64_t curPrefs = curPS->phy[i] & 0xFFF00000001FFFFFULL;
				MemoryReserveManager::lock(); // Lock MemoryReserveManager since the page tree is unstable.
				PageTreeNode* const newChild = (PageTreeNode*)MemoryReserveManager::alloc(8192, 8192); // First, modify the page tree so that it will stay semantically the same, but without the pagesize bit, so that we get more granular control.
				const uint64_t newChildPhy = lintophy(newChild);
				curPS->phy[i] = newChildPhy | (curPrefs & ~0x1080); // PAT and pagesize bits have different meanings for pagetable structures that don't map pages.
				curPS->lin[i] = newChild;
                for (int j=0;j<512;j++)
				{
                	newChild->phy[j] = curChildPhyRangeStart + childSize/512*j;
                	if (height == 2)
                		newChild->phy[j] |= curPrefs;
					else
					{
						kassert(height == 1);
						newChild->phy[j] |= (curPrefs & ~0x1080); // Pagesize and PAT bits have different meanings at height==0
						newChild->phy[j] |= (curPrefs & 0x1000) >> 5; // Set the PAT bit accordingly.
					}
				}
				MemoryReserveManager::unlock(); // Our caller shouldn't have free'd the range of linear memory we're currently paging out. So, this call to unlock() will not modify the part of page tree that is relevant to us.
                kassert(pageout(startLin, length, height-1, newChild, startLinCurPS + i*childSize)==false); // Then, recurse on that node. This part is the same as the else block below. TODO: Remove this duplication.
			}
		}
		else // If the current child does not map a page
		{
			if (pageout(startLin, length, height-1, curPS->lin[i], startLinCurPS + i*childSize)) // Recur into it
				curPS->phy[i] = 0;
		}
	}
	bool havePresentChild = false;
	for (int i=0;i<512;i++)
		if (curPS->phy[i] & 1)
		{
			havePresentChild = true;
			break;
		}
	if (havePresentChild == false)
	{
        MemoryReserveManager::free(curPS, 8192); // If none of our child tables exist, we're not used anymore.
        return true;
	}
	else
	{
		return false;
	}
}
void Pager::pagein(uint64_t startPhy, uint64_t startLin, uint64_t length, uint64_t prefs)
{
	kassert(startPhy % 4096 == 0);
	kassert(startLin % 4096 == 0);
	kassert(length   % 4096 == 0);
	kassert(lintophy((void*)startLin)==0);
	pagein(startPhy, startLin, length, 3, pml4, 0, prefs);
	kassert(lintophy((void*)startLin)==startPhy);
}
void Pager::pageout(void* startLin, uint64_t length)
{
	kassert((uint64_t)startLin % 4096 == 0);
	kassert(length   % 4096 == 0);
	pageout((uint64_t)startLin, length, 3, pml4, 0);
	kassert(lintophy(startLin)==0);
}

uint64_t Pager::lintophy(void* _p)
{
	uint64_t p = (uint64_t)_p;
    PageTreeNode* curNode = pml4;
    int height=3;
	while (1)
	{
		kassert(height >= 0);
		const int index = ((p>>12)>>(height*9))%512; // Index into the current table
        if ((curNode->phy[index] & 1) == 0) // If the child table isn't present, the page doesn't exist.
        	return 0;
        if ((height == 0) || (curNode->phy[index] & (1ULL<<7))) // If this entry maps a page
		{
			if (curNode->phy[index] & (1ULL<<7))
			{
				kassert(height < 3); // Pagesize bit is reserved for PML4e's.
				kassert(((curNode->phy[index]>>13) & ((1ULL<<(height*9-1))-1))==0); // Check if BMZ bits really are zero.
			}
			const int offset = p & ((1ULL<<(12+height*9))-1);
			uint64_t pageFrame = curNode->phy[index];
			pageFrame &= ~0xFFF0000000000FFFULL;
			if (height==1 || height==2)
				pageFrame &= ~(1ULL<<12); // PAT bit is not part of page frame address
			return pageFrame+offset;
		}
		else // If this entry points to a lower level table
		{
			curNode = curNode->lin[index];
			height--;
		}
	}
}

void Pager::initialize(void* _pml4)
{
	pml4 = (PageTreeNode*)_pml4;
}
