
#pragma once

#include <cstddef>
#include <type_traits>

namespace ostr
{
	template<class T>
	struct allocator
	{
		static T* allocate_single() noexcept
		{
			return new T;
		}
	
		static T* allocate_array(const size_t count) noexcept
		{
			return new T[count];
		}

		static void deallocate_single(const T* ptr) noexcept
		{
			delete ptr;
		}

		static void deallocate_array(const T* ptr) noexcept
		{
			delete[] ptr;
		}

		template<class...Args>
		static void placement_construct(void* ptr, Args&&...args) noexcept
		{
			new(ptr) T{ std::forward<Args>(args)... };
		}
	};
}
