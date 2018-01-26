#include "kout.h"

template<typename T, void(*D)(void *)>
shared_ptr<T, D>::shared_ptr():
	p(nullptr),
	cnt(nullptr)
{
}

template<typename T, void(*D)(void *)>
shared_ptr<T, D>::shared_ptr(T* _p):
	shared_ptr()
{
	reset(_p);
}

template<typename T, void(*D)(void *)>
shared_ptr<T, D>::shared_ptr(const shared_ptr& o):
	shared_ptr()
{
	(*this)=o;
}

template<typename T, void(*D)(void *)>
shared_ptr<T, D>::shared_ptr(shared_ptr&& o):
	shared_ptr()
{
	(*this)=o;
}

template<typename T, void(*D)(void *)>
shared_ptr<T, D>::~shared_ptr()
{
	reset();
}

template<typename T, void(*D)(void *)>
shared_ptr<T, D>& shared_ptr<T, D>::operator=(const shared_ptr& o)
{
	if (&o == this)
		return (*this);
	reset();
    p = o.p;
    cnt = o.cnt;
    if (cnt != nullptr)
    	(*cnt)++;
	return *(this);
}

template<typename T, void(*D)(void *)>
shared_ptr<T, D>& shared_ptr<T, D>::operator=(shared_ptr&& o)
{
	if (&o == this)
		return (*this);
	reset();
	p = o.p;
	cnt = o.cnt;
	o.p = nullptr;
	o.cnt = nullptr;
	return *(this);
}

template<typename T, void(*D)(void *)>
long shared_ptr<T, D>::use_count() const
{
	if (cnt==nullptr)
		return 0;
	return (*cnt);
}

template<typename T, void(*D)(void *)>
void shared_ptr<T, D>::reset()
{
	if (cnt != nullptr)
	{
		kassert(p != nullptr);
    	(*cnt)--;
    	if (*cnt == 0)
		{
			delete cnt;
			D(p);
			cnt = nullptr;
			p = nullptr;
		}
	}
	else
		kassert(p==nullptr);
}

template<typename T, void(*D)(void *)>
void shared_ptr<T, D>::reset(T* _p)
{
    if (p==_p)
    	return ;
	reset();
	p = _p;
	if (p != nullptr)
		cnt = new int64_t(1);
	else
		cnt = nullptr;
}

template<typename T, void(*D)(void *)>
T* shared_ptr<T, D>::get() const
{
	return p;
}

template<typename T, void(*D)(void *)>
T& shared_ptr<T, D>::operator*() const
{
	return (*p);
}

template<typename T, void(*D)(void *)>
T* shared_ptr<T, D>::operator->() const
{
	return p;
}

template<typename T, void(*D)(void *)>
T& shared_ptr<T, D>::operator[](int64_t index)
{
	return p[index];
}

template<typename T, void(*D)(void *)>
shared_ptr<T, D>::operator bool() const
{
	return p != nullptr;
}
