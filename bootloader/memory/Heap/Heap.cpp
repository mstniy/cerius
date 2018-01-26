#include "Heap.h"

#include "kout.h"
#include "utils.h"

static_assert(sizeof(AvlEntry<RangePair>) == sizeof(AvlEntry<SizePHPair>), "If sizeof(RangePair)!=sizeof(SizePHPair), we need seperate PrimitiveHeap's");
static_assert(sizeof(AvlEntry<RangePair>) == sizeof(AvlEntry<HeapObject>), "If sizeof(RangePair)!=sizeof(HeapObject), we need seperate PrimitiveHeap's");

PrimitiveHeap Heap::ph2048(256*1024, 2048, 2048, MemoryManager::alloc, MemoryManager::free, nullptr, 0, 128);
PrimitiveHeap Heap::phUIPair(2048, sizeof(AvlEntry<RangePair>), 8, Heap::alloc2048, Heap::free2048, nullptr, 0, 50);
PrimitiveHeap Heap::phPH(2048, sizeof(PrimitiveHeap), 8, Heap::alloc2048, Heap::free2048, nullptr, 0, 32);
RangeTree Heap::treeStart, Heap::treeLength;
AvlTree<HeapObject> Heap::singleObjects(Heap::phUIPair);
AvlTree<HeapObject> Heap::arrayObjects(Heap::phUIPair);
AvlTree<SizePHPair> Heap::pHeaps(Heap::phUIPair);
SizePHPair Heap::sppCache[5];
int Heap::sppCacheIndex = 0;

HeapObject::HeapObject(void* _ptr, size_t _size):
	ptr(_ptr),
	size(_size)
{
}

bool HeapObject::operator<(const HeapObject& o) const
{
    return ptr < o.ptr;
}

bool HeapObject::operator<=(const HeapObject& o) const
{
    return ptr <= o.ptr;
}

bool HeapObject::operator==(const HeapObject& o) const
{
	return ptr == o.ptr;
}

SizePHPair::SizePHPair(uint64_t _size, PrimitiveHeap* _ph):
	size(_size),
	ph(_ph)
{
}

bool SizePHPair::operator<(const SizePHPair& o) const
{
	return size < o.size;
}
bool SizePHPair::operator<=(const SizePHPair& o) const
{
	return size <= o.size;
}
bool SizePHPair::operator==(const SizePHPair& o) const
{
	return size == o.size;
}

PrimitiveHeap* Heap::getPH(size_t size)
{
	for (int i=0; i<5; i++)
		if (sppCache[i].size == size)
			return sppCache[i].ph;
	SizePHPair spp(size, nullptr);
	if (pHeaps.findBound(spp, false) && spp.size == size)
	{
		PrimitiveHeap* ph = spp.ph;
		sppCache[sppCacheIndex].size = size;
		sppCache[sppCacheIndex].ph = ph;
		sppCacheIndex = (sppCacheIndex+1) % 5;
		return ph;
	}
	else
		return nullptr;
}

void* Heap::allocSingleNoCreateEntry(size_t size)
{
	if (size==0)
		size=8;
    if (size % 8)
    	size += 8-size%8;
	if (size>128)
	{
		return allocArrayNoCreateEntry(size);
	}

	PrimitiveHeap* ph = getPH(size);
	if (ph == nullptr) // No PrimitiveHeap for that size was generated
	{
		ph = (PrimitiveHeap*)phPH.alloc(sizeof(PrimitiveHeap), 8);
		kassert(ph != nullptr);
		new (ph) PrimitiveHeap(2048, size, 8, Heap::alloc2048, Heap::free2048, nullptr, 0, 2048/size);
		pHeaps.add(SizePHPair(size, ph));
		sppCache[sppCacheIndex].size = size;
		sppCache[sppCacheIndex].ph = ph;
		sppCacheIndex = (sppCacheIndex+1) % 5;
	}
	void* res = ph->alloc(size, 8);
	return res;
}

void* Heap::allocSingle(size_t size)
{
    void* res=allocSingleNoCreateEntry(size);
    singleObjects.add(HeapObject(res, size));
	return res;
}

void* Heap::allocArray(size_t size)
{
	void* res = allocArrayNoCreateEntry(size);
	arrayObjects.add(HeapObject(res, size));
	return res;
}

void* Heap::allocArrayNoCreateEntry(size_t size)
{
	if (size==0)
		return allocSingleNoCreateEntry(0);
    if (size % 8)
    	size += 8-size%8;
	void* newPtr;
	if (size <= chunkSize/2)
	{
		RangePair p(size, 0);
		if (treeLength.findBound(p, false))
		{
			AddressSpaceManager::allocSubrangeFromSuperRange(treeStart, treeLength, RangePair(p.y, size), p);
			newPtr = (void*)p.y;
		}
		else
		{
			void* chunkPtr = MemoryManager::alloc(chunkSize, chunkSize);
			kassert(chunkPtr != nullptr);
			AddressSpaceManager::free(treeStart, treeLength, uint64_t(chunkPtr)+size, chunkSize-size);
			newPtr = chunkPtr;
		}
	}
	else
	{
		if (size % 4096)
			size += 4096-size%4096;
        newPtr = MemoryManager::alloc(size, 4096);
	}
	return newPtr;
}

void Heap::freeSingleNoDeleteEntry(void* p, size_t size)
{
	if (size==0)
		size=8;
    if (size % 8)
    	size += 8-size%8;
    if (size > 128)
    	freeArrayNoDeleteEntry(p, size);
	else
	{
		PrimitiveHeap* ph = getPH(size);
		kassert(ph != nullptr); // A PrimitiveHeap for that size must have been created.
    	ph->free(p, size);
    	// TODO: Use an intelligent strategy to figure out when to deallocate PrimitiveHeap's
	}
}

void Heap::freeSingle(void* p)
{
	HeapObject obj(p, 0);
	kassert(singleObjects.findBound(obj, false));
    kassert(obj.ptr==p); // Make sure that *p* actually exists on the heap
    kassert(singleObjects.del(obj));
	freeSingleNoDeleteEntry(p, obj.size);
}

void Heap::freeArrayNoDeleteEntry(void* p, size_t size)
{
	if (size==0)
	{
		freeSingleNoDeleteEntry(p, 0);
		return ;
	}
    if (size % 8)
    	size += 8-size%8;
	if (size <= chunkSize/2)
	{
		const RangePair newRange = AddressSpaceManager::free(treeStart, treeLength, (uint64_t)p, size);
		const uint64_t chunkStart = uint64_t(p) - uint64_t(p)%chunkSize;
		const uint64_t chunkEnd   = chunkStart+chunkSize;
       	if (newRange.x <= chunkStart && (newRange.x+newRange.y) >= chunkEnd) // If that chunk is completely empty
		{
			MemoryManager::free((void*)chunkStart, chunkSize); // TODO: Use an intelligent strategy to figure out when to deallocate chunks
       		AddressSpaceManager::alloc(treeStart, treeLength, RangePair(chunkSize, chunkStart));
		}
	}
	else
	{
		if (size % 4096)
			size += 4096-size%4096;
		MemoryManager::free(p, size);
	}
}

void Heap::freeArray(void* p)
{
    HeapObject obj(p, 0);
	kassert(arrayObjects.findBound(obj, false));
    kassert(obj.ptr==p); // Make sure that *p* actually exists on the heap
    kassert(arrayObjects.del(obj));
    freeArrayNoDeleteEntry(p, obj.size);
}

void* Heap::alloc2048(size_t size, size_t align)
{
	return ph2048.alloc(size, align);
}

void Heap::free2048(void* p, size_t size)
{
	ph2048.free(p, size);
}

void Heap::Init()
{
	new (&ph2048) PrimitiveHeap(256*1024, 2048, 2048, MemoryManager::alloc, MemoryManager::free, nullptr, 0, 128);
	new (&phUIPair) PrimitiveHeap(2048, sizeof(AvlEntry<RangePair>), 8, Heap::alloc2048, Heap::free2048, nullptr, 0, 50);
	new (&phPH) PrimitiveHeap(2048, sizeof(PrimitiveHeap), 8, Heap::alloc2048, Heap::free2048, nullptr, 0, 32);
	new (&treeStart) RangeTree(Heap::phUIPair);
	new (&treeLength) RangeTree(Heap::phUIPair);
	new (&singleObjects) AvlTree<HeapObject>(Heap::phUIPair);
	new (&arrayObjects) AvlTree<HeapObject>(Heap::phUIPair);
	new (&pHeaps) AvlTree<SizePHPair>(Heap::phUIPair);
	for (int i=0;i<5;i++)
	{
		kassert(sppCache[i].ph == nullptr);
		kassert(sppCache[i].size == 0);
	}
}
//#define HEAP_MONITOR_PERFORMANCE
#ifndef HEAP_MONITOR_PERFORMANCE
void* operator new(size_t size)
{
	return Heap::allocSingle(size);
}
void* operator new[](size_t size)
{
	return Heap::allocArray(size);
}
void operator delete(void* p)
{
	Heap::freeSingle(p);
}
void operator delete[](void* p)
{
	Heap::freeArray(p);
}
#else
void* operator new(size_t size)
{
	uint64_t t1, t2;
	t1 = rdtsc();
	void* res = Heap::allocSingle(size);
	t2 = rdtsc();
	kout << "new("<<size<<") took " << (t2-t1) << " cycles.\n";
	return res;
}
void* operator new[](size_t size)
{
	uint64_t t1, t2;
	t1 = rdtsc();
	void* res = Heap::allocArray(size);
	t2 = rdtsc();
	kout << "new[]("<<size<<") took " << (t2-t1) << " cycles.\n";
	return res;
}
void operator delete(void* p)
{
	uint64_t t1, t2;
	t1 = rdtsc();
	Heap::freeSingle(p);
	t2 = rdtsc();
	kout << "delete() took " << (t2-t1) << " cycles.\n";
}
void operator delete[](void* p)
{
	uint64_t t1, t2;
	t1 = rdtsc();
	Heap::freeArray(p);
	t2 = rdtsc();
	kout << "delete[]() took " << (t2-t1) << " cycles.\n";
}
#endif
