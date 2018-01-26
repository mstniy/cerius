template<typename T>
AvlTree<T>::AvlTree():
	root(NULL),
	pHeap(*(PrimitiveHeap*)nullptr)
{
}

template<typename T>
AvlTree<T>::AvlTree(PrimitiveHeap& _pHeap):
	root(NULL),
	pHeap(_pHeap)
{
}

template<typename T>
AvlTree<T>::~AvlTree()
{
	if (root != NULL)
		root->Destruct(pHeap);
	// pHeap doesn't belong to us, do not destruct it (not that we can properly do so, anyways).
}

template<typename T>
void AvlTree<T>::add(const T& t)
{
	if (root == NULL)
	{
		root = (AvlEntry<T>*)pHeap.alloc(sizeof(AvlEntry<T>), 8);
		new (root) AvlEntry<T>(t);
	}
	else
		root->add(t, pHeap);
}

template<typename T>
bool AvlTree<T>::del(const T& t)
{
	if (root == NULL)
		return false;
	else
	{
        std::pair<bool, bool> res = root->del(t, pHeap);
        if (res.second) // If the root has disappeared, delete it.
		{
			root->Destruct(pHeap);
        	root = NULL;
		}
		return res.first;
	}
}

template<typename T>
bool AvlTree<T>::findBound(T& t, bool upper)
{
	if (root == NULL)
		return false;
	return root->findBound(t, upper);
}

/*template<typename T>
bool AvlTree<T>::find(T& t)
{
    T tmp(t);
    if (findBound(t, true)==false) // If an element that is equal to *t* exists in the tree, findBound will return as soon as it finds it. So, using findBound is not slow.
		return false;
	return tmp==t;
}*/
