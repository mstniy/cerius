#include "AslrTree.h"

AslrTree::AslrTree(PrimitiveHeap& pHeap):
	AvlTree<RangePair>(pHeap)
{
}

void AslrTree::add(const RangePair& p)
{
	if (root == NULL)
	{
		root = (AslrEntry*)pHeap.alloc(sizeof(AslrEntry), 8);
		new (root) AslrEntry(p);
	}
	else
		root->add(p, pHeap);
}

bool AslrTree::findRandomRange(uint64_t size, uint64_t& start, RangePair& super)
{
	if (root == NULL)
		return false;
	else
	{
		uint64_t dummy;
		return static_cast<AslrEntry*>(root)->findRandomRange(size, start, dummy, super);
	}
}
