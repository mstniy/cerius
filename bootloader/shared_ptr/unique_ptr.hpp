#include "kout.h"

template<typename T, void(*D)(void *)>
unique_ptr<T, D>::unique_ptr():
	p(nullptr)
{
}

template<typename T, void(*D)(void *)>
unique_ptr<T, D>::unique_ptr(T* _p):
	unique_ptr()
{
	p = _p;
}

template<typename T, void(*D)(void *)>
unique_ptr<T, D>::unique_ptr(unique_ptr&& o):
	unique_ptr()
{
	p = o.p;
	o.p = nullptr;
}

template<typename T, void(*D)(void *)>
unique_ptr<T, D>::~unique_ptr()
{
	if (p != nullptr)
		D(p);
}

template<typename T, void(*D)(void *)>
unique_ptr<T, D>& unique_ptr<T, D>::operator=(unique_ptr&& o)
{
	if (&o == this)
		return (*this);
	if (p != nullptr)
		D(p);
	p = o.p;
	o.p = nullptr;
	return *(this);
}

template<typename T, void(*D)(void *)>
void unique_ptr<T, D>::reset()
{
	if (p != nullptr)
	{
		D(p);
		p = nullptr;
	}
}

template<typename T, void(*D)(void *)>
void unique_ptr<T, D>::reset(T* _p)
{
    if (p==_p)
    	return ;
	if (p != nullptr)
		D(p);
	p = _p;
}

template<typename T, void(*D)(void *)>
T* unique_ptr<T, D>::get() const
{
	return p;
}

template<typename T, void(*D)(void *)>
T& unique_ptr<T, D>::operator*() const
{
	return (*p);
}

template<typename T, void(*D)(void *)>
T* unique_ptr<T, D>::operator->() const
{
	return p;
}

template<typename T, void(*D)(void *)>
T& unique_ptr<T, D>::operator[](int64_t index)
{
	return p[index];
}

template<typename T, void(*D)(void *)>
unique_ptr<T, D>::operator bool() const
{
	return p != nullptr;
}
