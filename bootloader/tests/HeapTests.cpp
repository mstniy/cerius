#include "kassert.h"
#include "kout.h"
#include "memory/Heap/Heap.h"


unsigned lhash(unsigned x)
{
    int stable[16]={8, 12, 5, 11, 0, 3, 9, 14, 10, 2, 7, 15, 6, 13, 1, 4};
    unsigned res=0;
    for (int i=0;i<8;i++)
	{
		res<<=4;
		res|=((res>>4)%16) ^ (stable[x%16]);
		x>>=4;
	}
	return res;
}

uint64_t bhash(unsigned x)
{
	uint64_t res1 = lhash(lhash(x));
	uint64_t res2 = lhash(res1);
	return (res1<<32)|res2;
}

void HeapStressTest(int cycle)
{
	const int N = 1000;
	kout << "Heap stress test cycle " << cycle << ".\n";
	void** zeroes1 = new void*[N];
	void** zeroes2 = new void*[N];
	int**      ints = new int*[N];
	uint64_t** ones = new uint64_t*[N];
	uint64_t** twos = new uint64_t*[N];
	uint64_t** ttwos = new uint64_t*[N];
	uint64_t** otns = new uint64_t*[N];
	uint64_t** tts = new uint64_t*[N];
	uint64_t** ets = new uint64_t*[N];
	uint64_t** stts = new uint64_t*[N];
	kout << "Allocating... ";
	for (int i=0;i<N;i++)
	{
		zeroes1[i]=Heap::allocSingle(0);
		zeroes2[i]=Heap::allocArray(0);
		ints[i]=new int;kassert(ints[i]!=nullptr);
		ones[i]=new uint64_t;kassert(ones[i]!=nullptr);
    	twos[i]= new uint64_t[2];kassert(twos[i]!=nullptr);
    	ttwos[i] = new uint64_t[32];kassert(ttwos[i]!=nullptr);
		otns[i] = new uint64_t[129];kassert(otns[i]!=nullptr);
    	tts[i] = new uint64_t[2049];kassert(tts[i]!=nullptr);
    	ets[i] = new uint64_t[8193];kassert(ets[i]!=nullptr);
    	stts[i] = new uint64_t[18385];kassert(stts[i]!=nullptr);
	}
	kout << "Writing... ";
	int cur=bhash(cycle);
	for (int i=0;i<N;i++)
	{
		for (int j=0;j<1;j++) ints[i][j]=lhash(cur++);
        for (int j=0;j<1;j++) ones[i][j]=bhash(cur++);
        for (int j=0;j<2;j++) twos[i][j]=bhash(cur++);
        for (int j=0;j<32;j++) ttwos[i][j]=bhash(cur++);
        for (int j=0;j<129;j+=3) otns[i][j]=bhash(cur++);
        for (int j=0;j<2049;j+=10) tts[i][j]=bhash(cur++);
        for (int j=0;j<8193;j+=40) ets[i][j]=bhash(cur++);
        for (int j=0;j<18385;j+=150) stts[i][j]=bhash(cur++);
	}
	kout << "Reading... ";
	cur=bhash(cycle);
	for (int i=0;i<N;i++)
	{
		for (int j=0;j<1;j++) kassert(ints[i][j]==lhash(cur++));
        for (int j=0;j<1;j++) kassert(ones[i][j]==bhash(cur++));
        for (int j=0;j<2;j++) kassert(twos[i][j]==bhash(cur++));
        for (int j=0;j<32;j++) kassert(ttwos[i][j]==bhash(cur++));
        for (int j=0;j<129;j+=3) kassert(otns[i][j]==bhash(cur++));
        for (int j=0;j<2049;j+=10) kassert(tts[i][j]==bhash(cur++));
        for (int j=0;j<8193;j+=40) kassert(ets[i][j]==bhash(cur++));
        for (int j=0;j<18385;j+=150) kassert(stts[i][j]==bhash(cur++));
	}
	kout << "Freeing... ";
	for (int i=0;i<N;i++)
	{
		Heap::freeSingle(zeroes1[i]);
		Heap::freeArray(zeroes2[i]);
		delete ints[i];
		delete ones[i];
		delete[] twos[i];
		delete[] ttwos[i];
		delete[] otns[i];
		delete[] tts[i];
		delete[] ets[i];
		delete[] stts[i];
	}
	delete[] zeroes1;
	delete[] zeroes2;
	delete[] ints;
	delete[] ones;
	delete[] twos;
	delete[] ttwos;
	delete[] otns;
	delete[] tts;
	delete[] ets;
	delete[] stts;
	kout << "Done! (" << PhysicalSpaceManager::getAvailableMemory() << " bytes free)\n";
}

class C1
{
public:
	int x, y, z;
	C1* operator=(int i){x=lhash(i);y=lhash(x);z=lhash(y);}
	bool operator==(int i){return x==lhash(i) && y==lhash(x) && z==lhash(y);}
};

class C2
{
public:
	int x, y, z;
	char d;
	C2* operator=(int i){x=lhash(i);y=lhash(x);z=lhash(y);d=lhash(z);}
	bool operator==(int i){return x==lhash(i) && y==lhash(x) && z==lhash(y) && d==(char)lhash(z);}
};

class C3
{
public:
    uint64_t x, y, z, t, w;
    char a,b,c;
    int k,l,m;
    C3* operator=(int i){x=lhash(i);y=lhash(x);z=lhash(y);t=lhash(z);w=lhash(t);a=lhash(w);b=lhash(a);c=lhash(b);k=lhash(c);l=lhash(k);m=lhash(l);}
	bool operator==(int i){return x==lhash(i)&&y==lhash(x)&&z==lhash(y)&&t==lhash(z)&&w==lhash(t)&&a==(char)lhash(w)&&b==(char)lhash(a)&&c==(char)lhash(b)&&k==lhash(c)&&l==lhash(k)&&m==lhash(l);}
};

class C4
{
public:
	int cnt;
    int arr[16];
    C4* operator=(int i){cnt=lhash(i);arr[0]=lhash(cnt);for (int i=1;i<16;i++)arr[i]=lhash(arr[i-1]);}
	bool operator==(int i)
	{
		if(cnt!=lhash(i) || arr[0]!=lhash(cnt)) return false;
		for (int i=1;i<16;i++)if (arr[i]!=lhash(arr[i-1])) return false;
		return true;
	}
};

class C5
{
public:
	uint64_t cnt;
	uint64_t arr[16];
	C5* operator=(int i){cnt=lhash(i);arr[0]=lhash(cnt);for (int i=1;i<16;i++)arr[i]=lhash(arr[i-1]);}
	bool operator==(int i)
	{
		if(cnt!=lhash(i) || arr[0]!=lhash(cnt)) return false;
		for (int i=1;i<16;i++)if (arr[i]!=lhash(arr[i-1])) return false;
		return true;
	}
};

class C6
{
public:
	uint64_t x;
	C6* operator=(int _x){x=bhash(_x);}
	bool operator==(int _x){return x==bhash(_x);}
};

void HeapTypesTest(int cycle)
{
	const int N = 1000;
	kout << "Heap types test cycle " << cycle << ".\n";
	C1** c1s = new C1*[N];
	C2** c2s = new C2*[N];
	C3** c3s = new C3*[N];
	C4** c4s = new C4*[N];
	C5** c5s = new C5*[N];
	C6** c6s = new C6*[N];
	kout << "Allocating... ";
	for (int i=0;i<N;i++)
	{
		c1s[i]=new C1;kassert(c1s[i]!=nullptr);
		c2s[i]=new C2;kassert(c2s[i]!=nullptr);
		c3s[i]=new C3;kassert(c3s[i]!=nullptr);
		c4s[i]=new C4;kassert(c4s[i]!=nullptr);
		c5s[i]=new C5;kassert(c5s[i]!=nullptr);
		c6s[i]=new C6;kassert(c6s[i]!=nullptr);
	}
	kout << "Writing... ";
	int cur=bhash(cycle);
	for (int i=0;i<N;i++)
	{
		c1s[i][0]=bhash(cur++);
		c2s[i][0]=bhash(cur++);
		c3s[i][0]=bhash(cur++);
		c4s[i][0]=bhash(cur++);
		c5s[i][0]=bhash(cur++);
		c6s[i][0]=bhash(cur++);
	}
	kout << "Reading... ";
	cur=bhash(cycle);
	for (int i=0;i<N;i++)
	{
		kassert(c1s[i][0]==bhash(cur++));
		kassert(c2s[i][0]==bhash(cur++));
		kassert(c3s[i][0]==bhash(cur++));
		kassert(c4s[i][0]==bhash(cur++));
		kassert(c5s[i][0]==bhash(cur++));
		kassert(c6s[i][0]==bhash(cur++));
	}
	kout << "Freeing... ";
	for (int i=0;i<N;i++)
	{
		delete c1s[i];
		delete c2s[i];
		delete c3s[i];
		delete c4s[i];
		delete c5s[i];
		delete c6s[i];
	}
	delete[] c1s;
	delete[] c2s;
	delete[] c3s;
	delete[] c4s;
	delete[] c5s;
	delete[] c6s;
	kout << "Done! (" << PhysicalSpaceManager::getAvailableMemory() << " bytes free)\n";
}
