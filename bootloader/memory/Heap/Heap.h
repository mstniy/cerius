#ifndef HEAP_H
#define HEAP_H

#include "../PrimitiveHeap/PrimitiveHeap.h"
#include "../AvlTree/AvlTree.h"
#include "../AddressSpaceManager/AddressSpaceManager.h"
#include "../MemoryManager/MemoryManager.h"

class HeapObject
{
public:
	void* ptr;
	size_t size;
	HeapObject(void* _ptr, size_t _size);
	HeapObject(const HeapObject&) = default;
	bool operator<(const HeapObject& o) const;
	bool operator<=(const HeapObject& o) const;
	bool operator==(const HeapObject& o) const;
};

class SizePHPair // TODO: SizePHPair is almost identical to HeapObject, yet we instantiate AvlTree seperately, increasing binary size.
{
public:
    size_t size;
    PrimitiveHeap* ph;
    SizePHPair() = default;
    SizePHPair(uint64_t _size, PrimitiveHeap* _ph);
    SizePHPair(const SizePHPair&) = default;
	bool operator<(const SizePHPair& o) const;
	bool operator<=(const SizePHPair& o) const;
	bool operator==(const SizePHPair& o) const;
};

class Heap
{
private:
	static constexpr int chunkSize = 512*1024;
	static PrimitiveHeap phUIPair; // Keeps AvlEntry<RangePair>, AvlEntry<HeapObject> and AvlEntry<SizePHPair> (since all of them are of the same size)
	static PrimitiveHeap ph2048;
	static PrimitiveHeap phPH; // Keeps PrimitiveHeap's, because 'operator new' creates new PrimitiveHeap's dynamically.
    static RangeTree treeStart, treeLength; // Keeps track of address ranges allocated by the Heap
    static AvlTree<HeapObject> singleObjects; // Keeps objects allocated using 'allocSingle'
    static AvlTree<HeapObject> arrayObjects; // Keeps objects allocated using 'allocArray'
    // Each call to 'operator new' (not 'operator new[]') creates a new PrimitiveHeap tailored to the size of the type being created (if one doesn't exist, and the given size isn't too large).
    static AvlTree<SizePHPair> pHeaps;
    static SizePHPair sppCache[5];
    static int sppCacheIndex;
    static int zeroLengthDummy;
private:
	static void* alloc2048(size_t size, size_t align);
	static void free2048(void* p, size_t size);
	static void* allocSingleNoCreateEntry(size_t size);
	static void* allocArrayNoCreateEntry(size_t size);
	static void freeSingleNoDeleteEntry(void* p, size_t size);
	static void freeArrayNoDeleteEntry(void* p, size_t size);
	static PrimitiveHeap* getPH(size_t size);
public:
    Heap() = delete;
    ~Heap() = delete;
    Heap(Heap&&) = delete;
    Heap(const Heap&) = delete;
    static void Init();
	static void* allocSingle(size_t size); // To allocate a single object, use this function. It will create and use a PrimitiveHeap if possible, making allocations fast and efficient.
	static void* allocArray(size_t size);  // To allocate an array of objects, use this function.
	static void freeSingle(void* p);
	static void freeArray(void* p);
};

void* operator new(size_t size);
void operator delete(void* p);
void* operator new[](size_t size);
void operator delete[](void* p);

#endif
