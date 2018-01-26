#ifndef AVLENTRY_H
#define AVLENTRY_H

#include <algorithm> // std::max
#include "kassert.h"
#include <stdint.h>
#include "common/utils.h"
#include "../PrimitiveHeap/PrimitiveHeap.h"

template<typename T>
class AvlEntry
{
protected:
	AvlEntry(const T& _t);
	~AvlEntry();
	virtual AvlEntry* Clone(const T& t, PrimitiveHeap& pHeap); // Allocates and constructs a new AvlEntry.
	virtual void Destruct(PrimitiveHeap& pHeap);
protected:
	AvlEntry *e[2]; // e[0]: left child, e[1]: right child
	T t;
	int  height; // Leaves have a height of 1, internal nodes have height max(e[0].height, e[1].height)+1
protected:
	static int getHeight(const AvlEntry* n);
	virtual void invcache();
	void rebalance();
	void add(const T& tnew, PrimitiveHeap& pHeap); // *add* and *del* uses the given PrimitiveHeap to satisfy their dynamic memory requirements. Each AvlTree class keeps a reference to the PrimaryHeap associated with it, and passes this reference to its entries during each add/del call. We can't keep a reference in every AvlEntry, because that would be expensive.
	std::pair<bool, bool> del(const T& tnew, PrimitiveHeap& pHeap); // result.first: Whether *tnew* was found in the tree | result.second: Whether the current node has disappeared as a result of the operation.
	bool replacePtrWithGreatestOrSmallest(bool smallest, T& tnew, PrimitiveHeap& pHeap); // result: Whether the current node has disappeared as a result of the operation.
	bool findBound(T& t, bool upper); // See documentation of AvlTree::findBound

	template<typename> friend class AvlTree;
	friend class AslrTree;
	friend class AslrEntry;
};

#include "AvlEntry.hpp"

#endif
