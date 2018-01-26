#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include "../memory/Heap/Heap.h"

template<typename T, void (*D)(void*)=Heap::freeSingle>
class shared_ptr
{
private:
	T* p;
	int64_t* cnt;
public:
	shared_ptr();
	explicit shared_ptr(T* _p);
	shared_ptr(const shared_ptr&);
	shared_ptr(shared_ptr&&);
	~shared_ptr();
	shared_ptr& operator=(const shared_ptr&);
	shared_ptr& operator=(shared_ptr&&);
	long use_count() const;
	void reset();
	void reset(T* _p);
	T* get() const;
	T& operator*() const;
	T* operator->() const;
	T& operator[](int64_t index);
	explicit operator bool() const;
};

template<typename T>
using shared_array = shared_ptr<T, Heap::freeArray>;

#include "shared_ptr.hpp"

#endif
