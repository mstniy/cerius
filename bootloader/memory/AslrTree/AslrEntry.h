#ifndef ASLRENTRY_H
#define ASLRENTRY_H

#include <string>
#include <algorithm> // std::max
#include "kassert.h"
#include <stdint.h>
#include "../AvlTree/AvlEntry.h"
#include "RangePair.h"

class AslrEntry : public AvlEntry<RangePair>
{
private:
	uint64_t xTotal;
	int numChildren;
private:
    static bool rdrand_raw(uint64_t& r);
    static uint64_t rdrand();
    static void findNThSuitableByteInSubtree(AslrEntry* root, int64_t n, uint64_t size, uint64_t& start, RangePair& super);
protected:
	using AvlEntry<RangePair>::height; // C++ requires that these are declared here again, for some reason.
	using AvlEntry<RangePair>::e;
	using AvlEntry<RangePair>::t;
protected:
	AslrEntry(const RangePair& _p); // In order to create or destruct a AslrEntry, use construct and destruct methods. We must have a constructor so that the vtable can get initialized.
	virtual AvlEntry<RangePair>* Clone(const RangePair& _p, PrimitiveHeap& pHeap);
	virtual void Destruct(PrimitiveHeap& pHeap);
	virtual void invcache();
	// Finds a range of available addresses that is at least as long as 'size'
	bool findRandomRange(uint64_t size, uint64_t& start, uint64_t& among, RangePair& super); // among: Among how many possible start positions was this particular position was selected

	friend class AslrTree;
};

#endif
