#ifndef ASLRTREE_H
#define ASLRTREE_H

#include <algorithm> // std::max
#include "kassert.h"
#include <stdint.h>
#include "AslrEntry.h"
#include "../AvlTree/AvlTree.h"
#include "../PrimitiveHeap/PrimitiveHeap.h"

class AslrTree : public AvlTree<RangePair>
{
protected:
	using AvlTree<RangePair>::root;
public:
	AslrTree(PrimitiveHeap& pHeap);
	~AslrTree() = default;
	virtual void add(const RangePair& p);
	bool findRandomRange(uint64_t size, uint64_t& start, RangePair& super); // Finds a random range that has at least 'size' many consecutive available bytes.														            	  // Returns the start of the range, and the pair that describes the AvlEntry in which this range was found, to make the actual allocation process faster. (length in x, start in y)
};

#endif
