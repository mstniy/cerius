#include "PrimitiveHeap.h"

#include "common/kout.h"

extern bool enableBreakpoints;

PrimitiveHeap::PrimitiveHeap(int _chunkSize, int _objectSize, int _align, Allocator _allocator, Deallocator _deallocator, void* initChunk, int _minUnits, int _maxUnits):
	chunkSize(_chunkSize),
	objectSize(_objectSize),
	align(_align),
	allocator(_allocator),
	deallocator(_deallocator),
	nonempty_ll_root(NULL),
	empty_ll_root(NULL),
	numAvailableUnits(0),
	lockCnt(0),
	minUnits(_minUnits),
	maxUnits(_maxUnits)
{
	kassert(chunkSize   % 8 == 0);
	kassert(objectSize % 8 == 0);
	kassert(align      % 8 == 0);
	kassert((uint64_t)initChunk  % chunkSize == 0);
	kassert(objectSize % align == 0); // This condition makes sure that once we align the first object, the rest is aligned, too, without any extra padding.
	kassert(chunkSize   % align == 0);
    kassert(objectSize <= chunkSize-4*8);
    if ((3*8)%align != 0)
    	paddingSize = align-(3*8)%align; // All of the PrimitiveHeapChunk's we allocate are chunkSize aligned. There are 3 64-bit fields before the first object. Make sure that the first object is aligned properly.
	else
		paddingSize = 0;
    eLength = findOptimalELength();
    kassert(eLength != 1); // This may cause various problems throughout the code.
    kassert(maxUnits - minUnits >= eLength);
    kassert(eLength > 0 && eLength > minUnits);
    bmLength = (eLength+63)/64; // Round up
    if (initChunk != NULL)
    	replenish((PrimitiveHeapChunk*)initChunk);
	//kout << "PrimitiveHeap<" << objectSize << "> : eLength: " << eLength << "bmLength: " << bmLength << "paddingSize: " << paddingSize << "\n";
}

PrimitiveHeap::PrimitiveHeap(PrimitiveHeap&& o)
{
	chunkSize = o.chunkSize;
	objectSize = o.objectSize;
	align = o.align;
	allocator = o.allocator;
	deallocator = o.deallocator;
	nonempty_ll_root = o.nonempty_ll_root;
	o.nonempty_ll_root = nullptr;
	empty_ll_root = o.empty_ll_root;
	o.empty_ll_root = nullptr;
	numAvailableUnits = o.numAvailableUnits;
	o.numAvailableUnits = 0;
	lockCnt = o.lockCnt;
	o.lockCnt = 0;
	minUnits = o.minUnits;
	maxUnits = o.maxUnits;
	paddingSize = o.paddingSize;
    eLength = o.eLength;
    bmLength = o.bmLength;
}

PrimitiveHeap::~PrimitiveHeap()
{
    kassert(nonempty_ll_root == nullptr);
    while (empty_ll_root != nullptr)
	{
		kassert(lockCnt == 0);
		deallocator(empty_ll_root, chunkSize);
		empty_ll_root = empty_ll_root->next;
	}
}

uint64_t* PrimitiveHeap::findBeginningOfBMArray(PrimitiveHeapChunk* chunk)
{
    return (uint64_t*)(uint64_t(chunk)+3*8+paddingSize+objectSize*eLength);
}

uint64_t  PrimitiveHeap::findBeginningOfEArray(PrimitiveHeapChunk* chunk)
{
	return uint64_t(chunk)+3*8+paddingSize;
}

int PrimitiveHeap::findOptimalELength()
{
	// In a PrimitiveHeapChunk, there are two pointers to other PrimitiveHeapChunk's, for the double linked-list.
	// There's also the 'numavailableentry' field, which occupies 8 bytes.
	// There (probably) is some padding between numavailableentry and the first object to satisfy alignment requirements of the caller.
	// After the objects, there are bitmaps that describe which slots are available. They occupy ceil(x/64)*8 bytes.
	// So, we need the largest possible x such that 3*8+paddingSize+x*objectSize+ceil(x/64)*8<=chunkSize
	// The formula below was derived using the inequality x/64<=ceil(x/64)<=x/64+1
	int lim=(8*(chunkSize-3*8-paddingSize))/(8*objectSize+1);
	if (3*8+paddingSize+lim*objectSize+(lim+63)/64*8<=chunkSize)
		return lim;
	lim--;
	kassert(3*8+paddingSize+lim*objectSize+(lim+63)/64*8<=chunkSize);
	return lim;
}

void PrimitiveHeap::trim()
{
    while ((empty_ll_root != NULL) && (numAvailableUnits > maxUnits))
	{
		PrimitiveHeapChunk* oldRoot = empty_ll_root;
		removeChunkFromLL(oldRoot, empty_ll_root);
		numAvailableUnits -= eLength;
		deallocator(oldRoot, chunkSize); // The big PrimitiveHeap will use MemoryManager to deallocate empty chunks, which will in turn use the big PrimitiveHeap for any possible memory needs. PrimitiveHeap must be prefectly stable by the time it calls the Deallocator.
	}
}

void PrimitiveHeap::replenish()
{
	PrimitiveHeapChunk* chunk = (PrimitiveHeapChunk*)allocator(chunkSize, chunkSize);
	replenish(chunk);
}

void PrimitiveHeap::replenish(PrimitiveHeapChunk* chunk)
{
	kassert((uint64_t)chunk % chunkSize == 0);

	uint64_t* bm = findBeginningOfBMArray(chunk);

	for (int i=0; i<bmLength-1; i++) // Initially, all of the entries are available.
        bm[i] = ~0ULL;
	if (eLength%64 == 0)
		bm[bmLength-1] = ~0ULL;
	else
		bm[bmLength-1] = (1ULL<<(eLength%64))-1;
	chunk->numavailableentry = eLength;
	numAvailableUnits += eLength;
	addChunkToLL(chunk, empty_ll_root);
}

void* PrimitiveHeap::alloc(uint64_t size, uint64_t _align)
{
	kassert(size == objectSize);
	kassert(_align == align);

	if ((numAvailableUnits <= minUnits) && (lockCnt == 0))
	{
		lock();
		replenish();
		unlock(); // This unlock() will not call trim, because in the constructor we assert that minUnits+eLength<=maxUnits
	}
	kassert(numAvailableUnits > 0);
	int index=-1;
	if (nonempty_ll_root == NULL) // If the nonempty list is empty, there must be at least one entry in the empty list.
	{ 							  // Remove the first entry in the empty list and make it the root (and only element) of the nonempty list.
		kassert(empty_ll_root != NULL);
		PrimitiveHeapChunk* chunk = empty_ll_root;
		removeChunkFromLL(chunk, empty_ll_root);
		addChunkToLL(chunk, nonempty_ll_root);
	}
	kassert(nonempty_ll_root != NULL);

	uint64_t* bm = findBeginningOfBMArray(nonempty_ll_root);

	for (int bmindex=0;bmindex<bmLength;bmindex++)
		if (bm[bmindex] != 0)
		{
			index = 64*bmindex + binarylog(leastSignificantSetBitOf(bm[bmindex]));
			bm[bmindex] &= ~leastSignificantSetBitOf(bm[bmindex]);
			break;
		}
	kassert(index != -1); // The reserve cannot be empty here, because we did a kassert(numAvailableUnits > 0) at the beginning of the function.
	nonempty_ll_root->numavailableentry--;
	numAvailableUnits--;

	void* res = (void*)(findBeginningOfEArray(nonempty_ll_root)+objectSize*index);
	if (nonempty_ll_root->numavailableentry == 0)
		removeChunkFromLL(nonempty_ll_root, nonempty_ll_root);
	kassert(uint64_t(res)%align == 0);
	return res;
}
void PrimitiveHeap::free(void* p, uint64_t size)
{
	kassert((uint64_t)p % align == 0);
	kassert(size == objectSize);
	PrimitiveHeapChunk* chunk = (PrimitiveHeapChunk*)((uint64_t)p-(uint64_t)p%chunkSize);
	uint64_t* bm = findBeginningOfBMArray(chunk);
	int index = ((uint64_t)p-findBeginningOfEArray(chunk))/objectSize;
	kassert(index < eLength);
	bm[index/64] |= 1ULL<<(index%64);
	chunk->numavailableentry++;
	numAvailableUnits++;
	if (chunk->numavailableentry == 1) // This chunk used to be outside of the ll. Add it to nonempty_ll
	{
		addChunkToLL(chunk, nonempty_ll_root);
	}
	if (chunk->numavailableentry == eLength) // If all of the entries in the chunk are available
	{
		removeChunkFromLL(chunk, nonempty_ll_root); // Remove the chunk from nonempty_ll (in the constructor, we assert eLength>1, so the entry is guaranteed to have existed in nonempty_ll)
		addChunkToLL(chunk, empty_ll_root); // And place it into the empty_ll
	}
	if (lockCnt == 0)
	{
		lock();
		trim();
		unlock();
	}
}

void PrimitiveHeap::lock()
{
    lockCnt++;
}

void PrimitiveHeap::unlock()
{
	kassert(lockCnt > 0);
    lockCnt--;
    if (lockCnt == 0)
	{
		lockCnt++;
        if (numAvailableUnits <= minUnits)
        	replenish();
		else if (numAvailableUnits > maxUnits)
			trim();
		lockCnt--; // Do not use unlock() here, to avoid infinite loop.
	}
}

void PrimitiveHeap::removeChunkFromLL(PrimitiveHeapChunk* chunk, PrimitiveHeapChunk* &root)
{
	if (chunk->prev == NULL)
	{
		kassert(chunk == root);
		root = chunk->next;
	}
	else
		chunk->prev->next = chunk->next;
	if (chunk->next != NULL)
		chunk->next->prev = chunk->prev;
}

void PrimitiveHeap::addChunkToLL(PrimitiveHeapChunk* chunk, PrimitiveHeapChunk* &root)
{
    chunk->prev = NULL;
    chunk->next = root;
    if (root != nullptr)
	{
		kassert(root->prev == nullptr);
		root->prev = chunk;
	}
	root = chunk;
}
