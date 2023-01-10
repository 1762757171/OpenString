#pragma once

#include "types.h"
#include "unicode.h"

NS_EASY_BEGIN

struct codepoint
{
	static constexpr i32 sequence_length = 4;
	std::array<char, sequence_length> sequence;

#pragma region constructors
	
	explicit constexpr codepoint(const char v0 = 0, const char v1 = 0, const char v2 = 0, const char v3 = 0) noexcept
		: sequence({ v0, v1, v2, v3 })
	{ }

	explicit constexpr codepoint(const char* const v, const i32 length) noexcept
		: sequence()
	{
		for(i32 i = 0; i < length; ++i)
			sequence[i] = v[i];
	}

	explicit constexpr codepoint(const char* v) noexcept
		: codepoint(v, unicode::parse_utf8_length(v[0]))
	{ }

	explicit constexpr codepoint(const char32_t cp) noexcept
		: sequence(unicode::utf32_to_utf8(cp))
	{ }

#pragma endregion constructors

#pragma region iterators

	struct const_iterator
	{
		constexpr const_iterator() noexcept = default;
		
		explicit constexpr const_iterator(const char* v) noexcept
			: value(v)
		{ }

		[[nodiscard]] constexpr const char& operator*() const noexcept
		{
			return *this->value;
		}
		
		[[nodiscard]] constexpr const char* data() const noexcept
		{
			return this->value;
		}

		[[nodiscard]] constexpr std::ptrdiff_t operator-(const const_iterator& rhs) const noexcept
		{
			return this->value - rhs.value;
		}
		
		constexpr const_iterator& operator+=(const std::ptrdiff_t diff) noexcept
		{
			this->value += diff;
			return *this;
		}

		constexpr const_iterator& operator-=(const std::ptrdiff_t diff) noexcept
		{
			return this->operator+=(-diff);
		}

		[[nodiscard]] constexpr const_iterator operator+(const std::ptrdiff_t diff) const noexcept
		{
			const_iterator tmp = *this;
			tmp += diff;
			return tmp;
		}

		[[nodiscard]] constexpr const_iterator operator-(const std::ptrdiff_t diff) const noexcept
		{
			const_iterator tmp = *this;
			tmp -= diff;
			return tmp;
		}

	    constexpr const_iterator& operator++() noexcept
		{
			++this->value;
			return *this;
		}

	    constexpr const_iterator operator++(int) noexcept
		{
			const const_iterator tmp = *this;
	        ++*this;
	        return tmp;
	    }

		constexpr const_iterator& operator--() noexcept
		{
			--this->value;
			return *this;
	    }

		constexpr const_iterator operator--(int) noexcept 
		{
			const const_iterator tmp = *this;
			--*this;
			return tmp;
		}

	    [[nodiscard]] constexpr bool operator==(const const_iterator& rhs) const noexcept
		{
			return this->value == rhs.value;
	    }

	    [[nodiscard]] constexpr bool operator!=(const const_iterator& rhs) const noexcept
		{
	        return !(*this == rhs);
	    }

	    [[nodiscard]] constexpr bool operator<(const const_iterator& rhs) const noexcept
		{
	        return this->value < rhs.value;
	    }
	 
	    [[nodiscard]] constexpr bool operator>(const const_iterator& rhs) const noexcept
		{
	        return rhs < *this;
	    }
	 
	    [[nodiscard]] constexpr bool operator<=(const const_iterator& rhs) const noexcept
		{
	        return rhs >= *this;
	    }
	 
	    [[nodiscard]] constexpr bool operator>=(const const_iterator& rhs) const noexcept
		{
	        return !(*this < rhs);
	   }
		
		const char* value = nullptr;
	};
	
	[[nodiscard]] constexpr const_iterator begin() const noexcept
	{
		return const_iterator{ this->sequence.data() };
	}

	[[nodiscard]] constexpr const_iterator end() const noexcept
	{
		return const_iterator{ this->sequence.data() + this->size() };
	}

	[[nodiscard]] constexpr const_iterator cbegin() const noexcept
	{
		return this->begin();
	}

	[[nodiscard]] constexpr const_iterator cend() const noexcept
	{
		return this->end();
	}

#pragma endregion iterators

	[[nodiscard]] constexpr const char* data() const noexcept
	{
		return this->sequence.data();
	}

	[[nodiscard]] constexpr i32 size() const noexcept
	{
		for(i32 i = 0; i < sequence_length; ++i)
			if(this->sequence[i] == 0)
				return i;
		return sequence_length;
	}

	[[nodiscard]] constexpr char32_t get_codepoint() const noexcept
	{
		return unicode::utf8_to_utf32(this->sequence.data(), this->size());
	}

	[[nodiscard]] explicit constexpr operator char32_t() const noexcept
	{
		return this->get_codepoint();
	}
};

[[nodiscard]] constexpr bool operator==(const codepoint& lhs, const codepoint& rhs) noexcept
{
	const i32 size_l = lhs.size();
	const i32 size_r = rhs.size();
	if(size_l != size_r)
		return false;
	for(i32 i = 0; i < size_l; ++i)
		if(lhs.sequence[i] != rhs.sequence[i])
			return false;
	return true;
}

[[nodiscard]] constexpr bool operator==(const codepoint& lhs, const char* rhs) noexcept
{
	for(i32 i = 0; i < lhs.size(); ++i)
		if(lhs.sequence[i] != rhs[i])
			return false;
	return true;
}

[[nodiscard]] constexpr bool operator==(const codepoint& lhs, const char32_t rhs) noexcept
{
	return lhs.get_codepoint() == rhs;
}

[[nodiscard]] constexpr bool operator==(const char* lhs, const codepoint& rhs) noexcept
{
	return rhs == lhs;
}

[[nodiscard]] constexpr bool operator!=(const codepoint& lhs, const codepoint& rhs) noexcept
{
	return !(lhs == rhs);
}

NS_EASY_END

inline namespace literal
{
	[[nodiscard]] constexpr easy::codepoint operator""_cp(const char c) noexcept
	{
		return easy::codepoint{ c };
	}
	
	[[nodiscard]] constexpr easy::codepoint operator""_cp(const char32_t c) noexcept
	{
		return easy::codepoint{ c };
	}
}
