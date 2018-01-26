#include "AslrEntry.h"

AslrEntry::AslrEntry(const RangePair& _p):
	AvlEntry<RangePair>(_p),
	xTotal(_p.x),
	numChildren(1)
{
}

AvlEntry<RangePair>* AslrEntry::Clone(const RangePair& _p, PrimitiveHeap& pHeap)
{
	AslrEntry* newEntry = (AslrEntry*)pHeap.alloc(sizeof(AslrEntry), 8);
	new (newEntry) AslrEntry(_p);
	return newEntry;
}

void AslrEntry::Destruct(PrimitiveHeap& pHeap)
{
	for (int i=0;i<2;i++)
		if (e[i] != NULL)
		{
			e[i]->Destruct(pHeap);
			e[i] = nullptr;
		}
	pHeap.free(this, sizeof(AslrEntry));
}

void AslrEntry::invcache()
{
	AvlEntry<RangePair>::invcache();

	xTotal = t.x;
	numChildren = 1;
	if (e[0] != NULL)
	{
		xTotal += static_cast<AslrEntry*>(e[0])->xTotal;
		numChildren += static_cast<AslrEntry*>(e[0])->numChildren;
	}
	if (e[1] != NULL)
	{
		xTotal += static_cast<AslrEntry*>(e[1])->xTotal;
		numChildren += static_cast<AslrEntry*>(e[1])->numChildren;
	}
}


bool AslrEntry::findRandomRange(uint64_t size, uint64_t& start, uint64_t& among, RangePair& super)
{
	AslrEntry* const ee[2] = {static_cast<AslrEntry*>(e[0]), static_cast<AslrEntry*>(e[1])};
    if (t.x<size)
	{
		if (ee[1] != NULL)
			return ee[1]->findRandomRange(size, start, among, super);
		else
			return false;
	}
	else // t.x >= size
	{
		RangePair leftSuper;
		uint64_t leftStart, leftAmong;
		const uint64_t thisAmong = t.x-size+1;
        uint64_t tot = thisAmong;
        if ((ee[0] != NULL) && ee[0]->findRandomRange(size, leftStart, leftAmong, leftSuper))
		{
			tot += leftAmong;
		}
		else
			leftAmong = 0;
		if (ee[1] != NULL)
            tot += ee[1]->xTotal-ee[1]->numChildren*(size-1); // For each of the ranges in the subtree to our right, we cannot choose the last (size-1) bytes because we need 'size' many consecutive bytes.
		const uint64_t rnd = rdrand() % tot;
		among = tot;
        if (rnd < leftAmong) // With leftAmong/tot probability, choose a random range from our left subtree.
        {
        	start = leftStart;
        	super = leftSuper;
        	return true;
        }
        else if (rnd < leftAmong + thisAmong) // With thisAmong/tot probability, choose a random range from our range.
		{
			start = t.y + rnd-leftAmong;
			super = t;
			return true;
		}
		else // Otherwise, choose a random range from our right subtree. We don't need to generate any more random numbers, since we know that all of the available ranges in out right subtree are at least as large as *size*
		{
			findNThSuitableByteInSubtree(
				ee[1],
				rnd-leftAmong-thisAmong,
				size,
				start,
				super);
            return true;
		}
	}
}

void AslrEntry::findNThSuitableByteInSubtree(AslrEntry* root, int64_t n, uint64_t size, uint64_t& start, RangePair& super)
{
	while (1)
	{
		kassert(n >= 0);
		kassert(root != nullptr);
		kassert(n < root->xTotal);
		kassert(root->t.x >= size);
		const int64_t leftSuitableXTotal = (root->e[0] == nullptr) ? 0 : (static_cast<AslrEntry*>(root->e[0])->xTotal - static_cast<AslrEntry*>(root->e[0])->numChildren*(size-1));
		kassert(leftSuitableXTotal >= 0);
		const uint64_t thisSuitableX = root->t.x-size+1;
		if (n < thisSuitableX)
		{
			start = root->t.y + n;
			super = root->t;
			break;
		}
		else if (n < thisSuitableX + leftSuitableXTotal)
		{
			n -= thisSuitableX;
			root = static_cast<AslrEntry*>(root->e[0]);
		}
		else
		{
			n -= thisSuitableX + leftSuitableXTotal;
			root = static_cast<AslrEntry*>(root->e[1]);
		}
	}
}

bool AslrEntry::rdrand_raw(uint64_t& r)
{
	/*uint64_t b=0;
	asm volatile("rdrand %[r]\n\t"
				 "jnc end%=\n\t"
				 "mov $1, %[b]\n\t"
				 "end%=:\n\t"
		: [r] "=r"(r), [b] "=rm"(b)
		:
		: "cc");
	return b;*/
	// Bochs doesn't seem to support rdrand.
    static uint64_t seed = 0xA7C021FB6484A2EBULL; // TODO: Use a proper PRNG here.
    seed ^= seed>>5;
    seed ^= seed<<5;
    seed += 0x1039847572921348;
    r=seed;
    return true;
}


uint64_t AslrEntry::rdrand()
{
	uint64_t t;
    for (int i=0;i<10;i++)
		if (rdrand_raw(t))
			return t;
	return 0;
}
