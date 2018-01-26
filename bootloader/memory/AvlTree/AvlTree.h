#ifndef AVLTREE_H
#define AVLTREE_H

#include <algorithm> // std::max
#include "kassert.h"
#include <stdint.h>
#include "AvlEntry.h"

template<typename T>
class AvlTree
{
protected:
	AvlEntry<T>* root;
protected:
	PrimitiveHeap& pHeap;
public:
	AvlTree();
	AvlTree(PrimitiveHeap& _pHeap);
	~AvlTree();
	virtual void add(const T &t); // This function is virtual because if the root doesn't exist AslrTree::add creates an AslrEntry as the root, while AvlTree::add creates an AvlEntry.
	bool del(const T &t); // Returns true if the given *T* was found in the tree.
	bool findBound(T& t, bool upper); // If upper==false, finds a T 'tnew' in the tree such that tnew is (one of) the smallest value(s) greater than (or equal to) t.
									  // If upper==true , finds a T 'tnew' in the tree such that tnew is (one of) the greatest value(s) less than (or equal to) t.
									  // Sets t=tnew, so *t* is both input and output variable.
	/*bool find(T& t); // Returns true if an element 'tnew' exists in the tree such that t==tnew.
					 // If there is such an element, sets t=tnew. *t* may change even when the function returns false.
	*/
};

#include "AvlTree.hpp"

#endif
