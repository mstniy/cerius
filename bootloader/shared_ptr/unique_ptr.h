#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

#include "memory/Heap/Heap.h"

template<typename T, void (*D)(void*)=Heap::freeSingle>
class unique_ptr
{
private:
	T* p;
public:
	unique_ptr();
	explicit unique_ptr(T* _p);
	unique_ptr(const unique_ptr&) = delete;
	unique_ptr(unique_ptr&&);
	~unique_ptr();
	unique_ptr& operator=(const unique_ptr&) = delete;
	unique_ptr& operator=(unique_ptr&&);
	void reset();
	void reset(T* _p);
	T* get() const;
	T& operator*() const;
	T* operator->() const;
	T& operator[](int64_t index);
	explicit operator bool() const;
};

template<typename T>
using unique_array = unique_ptr<T, Heap::freeArray>;

#include "unique_ptr.hpp"

#endif
