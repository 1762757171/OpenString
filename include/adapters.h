#pragma once
#include "definitions.h"

NS_EASY_BEGIN

template<class T>
struct allocator
{
	static T* allocate_single()
	{
		return new T;
	}
	
	static T* allocate_array(const size_t count)
	{
		return new T[count];
	}

	static void deallocate_single(const T* ptr)
	{
		delete ptr;
	}

	static void deallocate_array(const T* ptr)
	{
		delete[] ptr;
	}
};

NS_EASY_END
