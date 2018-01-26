template<typename T>
AvlEntry<T>::AvlEntry(const T& _t):
	t(_t),
	height(1),
	e{nullptr, nullptr}
{
}

template<typename T>
AvlEntry<T>* AvlEntry<T>::Clone(const T& _t, PrimitiveHeap& pHeap)
{
	AvlEntry* newEntry = (AvlEntry*)pHeap.alloc(sizeof(AvlEntry), 8);
	new (newEntry) AvlEntry(_t);
	return newEntry;
}

template<typename T>
void AvlEntry<T>::Destruct(PrimitiveHeap& pHeap)
{
	for (int i=0;i<2;i++)
		if (e[i] != NULL)
		{
			e[i]->Destruct(pHeap);
			e[i] = nullptr;
		}
	pHeap.free(this, sizeof(AvlEntry));
}

/*template<typename T>
AvlEntry<T>::~AvlEntry()
{
	kassert(e[0] == nullptr); // Since the PrimitiveHeap the current AvlEntry is associated with isn't kept inside AvlEntry, we have no way of knowing which PrimitiveHeap to call into here.
	kassert(e[1] == nullptr); // Thus, we cannot dynamically free memory.
							  // We can, however, check whether *Destruct* has been called previously, to ensure we aren't leaking memory somewhere in the code.
}*/

template<typename T>
int AvlEntry<T>::getHeight(const AvlEntry<T>* n)
{
	if (n==NULL)
		return 0;
	return n->height;
}

template<typename T>
void AvlEntry<T>::invcache()
{
	height = std::max(AvlEntry::getHeight(e[0]), AvlEntry::getHeight(e[1]))+1;
}

template<typename T>
void AvlEntry<T>::rebalance()
{
	const int lmr = AvlEntry::getHeight(e[0]) - AvlEntry::getHeight(e[1]);
	kassert(myabs(lmr) <= 2);
	if (myabs(lmr) == 2)
	{
		int left, right;
		if (lmr == 2)
			left = 0;
		else
			left = 1;
		right = 1-left;
		if (AvlEntry::getHeight(e[left]->e[left]) >= AvlEntry::getHeight(e[left]->e[right]))
		{
			AvlEntry* tmp = e[right];
            e[right] = e[left];
            e[left] = e[left]->e[left];
            e[right]->e[left] = e[right]->e[right];
            e[right]->e[right] = tmp;
			std::swap(t, e[right]->t);
            e[right]->invcache();
		}
		else
		{
			AvlEntry* tmp = e[right];
            e[right] = e[left]->e[right];
            e[left]->e[right] = e[right]->e[left];
            e[right]->e[left] = e[right]->e[right];
            e[right]->e[right] = tmp;
            std::swap(t, e[right]->t);
            e[left]->invcache();
            e[right]->invcache();
		}
	}
	invcache(); // add and del mutates our children. We need this call to flush stale information from caches.
	kassert(myabs(AvlEntry::getHeight(e[0]) - AvlEntry::getHeight(e[1])) <= 1);
}

template<typename T>
void AvlEntry<T>::add(const T& tnew, PrimitiveHeap& pHeap)
{
	kassert(myabs(AvlEntry::getHeight(e[0]) - AvlEntry::getHeight(e[1])) <= 1);

	int left, right;
	left = t <= tnew;
	right = 1-left;

	if (e[left] == NULL)
		e[left] = this->Clone(tnew, pHeap);
	else
		e[left]->add(tnew, pHeap);

	rebalance();
}

template<typename T>
std::pair<bool, bool> AvlEntry<T>::del(const T& tnew, PrimitiveHeap& pHeap)
{
	kassert(myabs(AvlEntry::getHeight(e[0]) - AvlEntry::getHeight(e[1])) <= 1);

	std::pair<bool, bool> res;
    if (tnew == t)
	{
		if (e[0] != nullptr || e[1] != nullptr)
		{
			int left, right;
			if (e[0] != NULL)
				left = 0;
			else
				left = 1;
			right = 1-left;

           	bool childres = e[left]->replacePtrWithGreatestOrSmallest(left, t, pHeap);
           	if (childres)
			{
				e[left]->Destruct(pHeap);
            	e[left] = NULL;
			}
			res = std::make_pair(true, false);
		}
		else
			res = std::make_pair(true, true);
	}
	else
	{
		int left ,right;
		left = t <= tnew;
		right = 1-left;

		if (e[left] == NULL)
			res = std::make_pair(false, false);
		else
		{
           	std::pair<bool, bool> childres = e[left]->del(tnew, pHeap);
           	if (childres.second)
			{
				e[left]->Destruct(pHeap);
	           	e[left] = NULL;
			}
			res = std::make_pair(childres.first, false);
		}
	}
	rebalance();
	return res;
}

template<typename T>
bool AvlEntry<T>::replacePtrWithGreatestOrSmallest(bool smallest, T& tt, PrimitiveHeap& pHeap)
{
	int left, right;
	if (smallest)
		left = 0;
	else
		left = 1;
	right = 1-left;

	bool res;
	if (e[left] != NULL)
	{
		if (e[left]->replacePtrWithGreatestOrSmallest(smallest, tt, pHeap))
		{
			e[left]->Destruct(pHeap);
			e[left] = NULL;
		}
		res = false;
	}
	else
	{
		tt = t;
        if (e[right] == NULL)
            res = true;
		else
		{
            if (e[right]->replacePtrWithGreatestOrSmallest(smallest, t, pHeap))
			{
				e[right]->Destruct(pHeap);
            	e[right] = NULL;
			}
            res = false;
		}
	}
	rebalance();
	return res;
}

template<typename T>
bool AvlEntry<T>::findBound(T& tnew, bool upper)
{
    if (t == tnew)
	{
		tnew = t;
		return true;
	}
	else if ((t < tnew) ^ upper)
	{
		if (e[!upper] == NULL)
			return false;
        return e[!upper]->findBound(tnew, upper);
	}
	else
	{
		T tmp(tnew);
        tnew = t;
        if (e[upper] != NULL)
            if (e[upper]->findBound(tmp, upper))
				tnew = tmp;
		return true;
	}
}
