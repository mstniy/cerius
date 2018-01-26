#ifndef PRIMITIVE_HEAP_H
#define PRIMITIVE_HEAP_H

#include "BitmapHelper.h"
#include <stdint.h>
#include "kassert.h"
#include "common/utils.h"

struct PrimitiveHeapChunk // Keep the code of PrimitiveHeap in-sync with this layout.
{
	PrimitiveHeapChunk* next, *prev;
	int64_t numavailableentry; // Number of available entries in this chunk.
	// The struct, in reality, doesn't end here. After this point, there's (probably) some alignment, the rest of the e array, and the bm array, in that order.
	// 'e' is an array of objects, each of which is 'objectSize' bytes
	// 'bm' is an array of uint64_t's. They're used as bitmaps describing which entries in the chunk is available.
};

/*
 A PrimitiveHeap is a heap that can only allocate objects of a fixed size, which is specified during its construction.
 PrimitiveHeap's are used by the physical and linear memory managers, to satisfy their AvlEntry and AslrEntry needs.
 PrimitiveHeap's are also used by the MemoryReserveManager to divide large chunks of memory it allocates using MemoryManager into smaller chunks of 4096 bytes, which are in turn used by the Pager as page table entries, and by the PrimitiveHeap's of physical and linear space managers.
 Being very space and time efficient, PrimitiveHeap is also used by the Heap for non-array objects of small-sized types.
*/

class PrimitiveHeap
{
public:
	using Allocator   = void*(*)(uint64_t size, uint64_t align);
	using Deallocator = void (*)(void* p, uint64_t size);
private:
	PrimitiveHeapChunk* nonempty_ll_root; // A double-linked-list of PrimitiveHeapChunks which have at least one available entry, but aren't completely empty either.
	PrimitiveHeapChunk* empty_ll_root; // A double-linked-list of PrimitiveHeapChunks which are completely empty. These chunks are to be free'd using the deallocator provided to the constructor, if numAvailableUnits increases above maxUnits.
	int chunkSize; // Size of each chunk, in bytes. The PrimitiveHeap allocates/deallocates this many bytes from/to the supplied allocator/deallocator whenever it needs to allocate/deallocate memory.
	int objectSize; // Size of each object, in bytes.
	int paddingSize; // The required size of padding in each chunk to satisfy the alignment requirement supplied to the constructor, in bytes.
	int bmLength; // Length of the 'bm' array in each PrimitiveHeapChunk managed by the current instance.
	int eLength; // Length of the 'e' array in each PrimitiveHeapChunk managed by the current instance.
	int numAvailableUnits; // Total number of available units in all chunks in both linked lists.
	int align; // Values returned by alloc() will be 'align' aligned.
	int lockCnt;
	int minUnits; // Every time unlock() or alloc() is called and lockCnt == 0 and numAvilableUnits <= minUnits, the heap will replenish itself.
	int maxUnits; // Every time unlock() or free() is called and lockCnt == 0 and numAvilableUnits > maxUnits, the heap will trim itself.
	Allocator allocator;
	Deallocator deallocator;
private:
	void replenish(PrimitiveHeapChunk* chunk); // Embraces the given PrimitiveHeapChunk.
	uint64_t* findBeginningOfBMArray(PrimitiveHeapChunk* chunk);
	uint64_t  findBeginningOfEArray(PrimitiveHeapChunk* chunk);
	int findOptimalELength();
	void addChunkToLL(PrimitiveHeapChunk* chunk, PrimitiveHeapChunk* &root);
	void removeChunkFromLL(PrimitiveHeapChunk* chunk, PrimitiveHeapChunk* &root);
	void replenish(); // Uses the supplied Allocator to allocate another chunk, increasing numAvailableUnits by eLength.
	void trim(); // Uses the supplied Deallocator to free the entries in empty_ll until no entry is left in empty_ll, or numAvailableUnits <= maxUnits
public:
	PrimitiveHeap() = default;
	PrimitiveHeap(int _chunkSize, int _objectSize, int _align, Allocator _allocator, Deallocator _deallocator, void* initChunk, int _minUnits, int _maxUnits);
	PrimitiveHeap(const PrimitiveHeap&) = delete;
	PrimitiveHeap(PrimitiveHeap&& o);
	~PrimitiveHeap();
	void* alloc(uint64_t size, uint64_t align); // Allocates a single entry, returns its address. 'size' must equal objectSize. 'align' must equal the alignment requirement passed to the constructor.
	void free(void* p, uint64_t size); // Free's the given entry. 'size' must equal objectSize
	void lock(); // Call lock() before the underlying Allocator/Deallocator becomes unstable. Before calling lock(), make sure that the heap has enough available units in it to last requests until the next unlock().
	void unlock(); // Call unlock() after the underlying Allocator/Deallocator becomes stable.
};

#endif
