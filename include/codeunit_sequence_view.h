#pragma once

#include "definitions.h"
#include "types.h"
#include "helpers.h"
#include "index_interval.h"
#include <tuple>

NS_EASY_BEGIN

class EASY_EXPORT codeunit_sequence_view
{
public:

#pragma region constructors

	constexpr codeunit_sequence_view() noexcept = default;
	constexpr codeunit_sequence_view(const codeunit_sequence_view&) noexcept = default;
	constexpr codeunit_sequence_view(codeunit_sequence_view&&) noexcept = default;
	constexpr codeunit_sequence_view& operator=(const codeunit_sequence_view& other) noexcept = default;
	constexpr codeunit_sequence_view& operator=(codeunit_sequence_view&& other) noexcept = default;
	~codeunit_sequence_view() noexcept = default;

	constexpr codeunit_sequence_view(const char* data, const i32 count) noexcept
		: size_(count)
		, data_(data)
	{ }

	constexpr codeunit_sequence_view(const char* data, const size_t count) noexcept
		: size_(static_cast<i32>(count))
		, data_(data)
	{ }

	constexpr codeunit_sequence_view(const char* data, const char* last) noexcept
		: size_(static_cast<i32>(last - data))
		, data_(data)
	{ }

	explicit constexpr codeunit_sequence_view(const char* str) noexcept
		: codeunit_sequence_view(str, helper::string::count_string_length(str))
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
		return const_iterator{ this->data_ };
	}

	[[nodiscard]] constexpr const_iterator end() const noexcept
	{
		return const_iterator{ this->data_ + this->size_ };
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
	
	[[nodiscard]] constexpr bool operator==(const codeunit_sequence_view& rhs) const noexcept
	{
		if (this->size_ != rhs.size_) 
			return false;
		for(i32 i = 0; i < this->size_; ++i)
			if(this->data_[i] != rhs.data_[i])
				return false;
		return true;
	}
	
	[[nodiscard]] constexpr bool operator==(const char* rhs) const noexcept
	{
		return this->operator==(codeunit_sequence_view(rhs));
	}

	[[nodiscard]] constexpr bool operator!=(const codeunit_sequence_view& rhs) const noexcept
	{
		return !this->operator==(rhs);
	}
	
	[[nodiscard]] constexpr bool operator!=(const char* rhs) const noexcept
	{
		return !this->operator==(rhs);
	}

	[[nodiscard]] constexpr i32 size() const noexcept
	{
		return this->size_;
	}

	[[nodiscard]] constexpr const char* data() const noexcept
	{
		return this->data_;
	}

	[[nodiscard]] constexpr const char* c_str() const noexcept
	{
		return this->data_;
	}

	[[nodiscard]] constexpr const char* last() const noexcept
	{
		return this->data_ + this->size_;
	}

	/// @return Is this an empty string
	[[nodiscard]] constexpr bool is_empty() const noexcept
	{
		return this->size_ == 0;
	}

	[[nodiscard]] constexpr codeunit_sequence_view subview(const index_interval& range) const noexcept
	{
		const i32 self_size = this->size();
		const index_interval selection = range.select(self_size);
		if(selection.is_empty())
			return { };
		const i32 size = selection.size();
		const char* first_data = this->data_ + selection.get_inclusive_min();
		return { first_data, size };
	}
	
	[[nodiscard]] constexpr const char& read_at(const i32 index) const noexcept
	{
		return this->data_[index + (index >= 0 ? 0 : this->size_)];
	}

	[[nodiscard]] constexpr const char& operator[](const i32 index) const noexcept
	{
		return this->read_at(index);
	}

	[[nodiscard]] constexpr codeunit_sequence_view operator[](const index_interval& range) const noexcept
	{
		return this->subview(range);
	}

	/**
	 * \brief this is learned from FBString in folly
	 * see https://github.com/facebook/folly/blob/main/folly/FBString.h
	 * which is a Boyer-Moore-like trick
	 * \param pattern string which to search in this string
	 * \param range search range
	 * \return index where searched, return index_invalid if not found
	 */
	[[nodiscard]] constexpr i32 index_of(const codeunit_sequence_view& pattern, const index_interval& range = index_interval::all()) const noexcept
	{
		if(pattern.is_empty())
			return index_invalid;
		const i32 self_size = this->size();
		const index_interval selection = range.select(self_size);
		const codeunit_sequence_view view = this->subview(selection);
		if(view.size() < pattern.size())
			return index_invalid;

		const char pattern_last = pattern.data_[pattern.size_ - 1];
		i32 skip = 1;
		while(pattern.size_ > skip && pattern.data_[pattern.size_ - 1 - skip] != pattern_last)
			++skip;

		i32 i = 0;
		const i32 endpoint = view.size_ - pattern.size_ + 1;
		while(i < endpoint)
		{
			while(true)
			{
				if(view.data_[i + pattern.size_ - 1] == pattern_last)
					break;
				++i;
				if(i == endpoint)
					return index_invalid;
			}
			if(view.subview({'[', i, i + pattern.size(),')' }) == pattern)
				return i + selection.get_inclusive_min();
			i += skip;
		}
		return index_invalid;
	}

	[[nodiscard]] constexpr i32 index_of(const char codeunit, const index_interval& range = index_interval::all()) const noexcept
	{
		if(codeunit == 0)
			return index_invalid;
		const i32 self_size = this->size();
		const index_interval selection = range.select(self_size);
		const codeunit_sequence_view view = this->subview(selection);
		for(i32 i = 0; i < view.size_; ++i)
			if(view.data_[i] == codeunit)
				return i + selection.get_inclusive_min();
		return index_invalid;
	}
	
	[[nodiscard]] constexpr i32 last_index_of(const codeunit_sequence_view& pattern, const index_interval& range = index_interval::all()) const noexcept
	{
		const i32 self_size = this->size();
		const index_interval selection = range.select(self_size);
		const codeunit_sequence_view view = this->subview(selection);
		if(pattern.is_empty())
			return index_invalid;
		if(view.size_ < pattern.size_)
			return index_invalid;

		const char pattern_first = pattern.data_[0];
		i32 skip = 1;
		while(pattern.size_ > skip && pattern.data_[skip] != pattern_first)
			++skip;

		i32 i = view.size_ - pattern.size_;
		constexpr i32 endpoint = 0;
		while(i >= endpoint)
		{
			while(true)
			{
				if(view.data_[i] == pattern_first)
					break;
				if(i == endpoint)
					return index_invalid;
				--i;
			}
			if(view.subview({'[', i, i + pattern.size(),')' }) == pattern)
				return i + selection.get_inclusive_min();
			i -= skip;
		}
		return index_invalid;
	}

	[[nodiscard]] constexpr i32 index_any_of(const codeunit_sequence_view& units, const index_interval& range = index_interval::all()) const noexcept
	{
		const index_interval selection = range.select(this->size());
		for(const i32 i : selection)
		{
			const char& unit = this->data_[i];
			if(units.contains(unit))
				return i;
		}
		return index_invalid;
	}

	[[nodiscard]] constexpr bool contains(const codeunit_sequence_view& pattern) const noexcept
	{
		return this->index_of(pattern) != index_invalid;
	}

	[[nodiscard]] constexpr bool contains(const char codeunit) const noexcept
	{
		return this->index_of(codeunit) != index_invalid;
	}

	[[nodiscard]] constexpr std::tuple<codeunit_sequence_view, codeunit_sequence_view> split(const codeunit_sequence_view& splitter) const noexcept
	{
		const i32 index = this->index_of(splitter);
		if(index == index_invalid)
			return { *this, { } };
		const codeunit_sequence_view first = this->subview({ '[', 0, index, ')' });
		const codeunit_sequence_view second = this->subview({ '[', index + splitter.size(), '~' });
		return { first, second };
	}

	[[nodiscard]] constexpr i32 count(const codeunit_sequence_view& pattern) const noexcept
	{
		codeunit_sequence_view view = *this;
		i32 count = 0;
		while(true)
		{
			const i32 index = view.index_of(pattern);
			if(index == index_invalid)
				break;
			++count;
			view = view.subview({ '(', index, '~' });
		}
		return count;
	}

	[[nodiscard]] constexpr bool starts_with(const codeunit_sequence_view& prefix) const noexcept
	{
		if(prefix.is_empty())
			return true;
		const i32 start_size = prefix.size_;
		return this->subview({ '~', start_size , ')' }) == prefix;
	}

	[[nodiscard]] constexpr bool ends_with(const codeunit_sequence_view& suffix) const noexcept
	{
		if(suffix.is_empty())
			return true;
		const i32 end_size = suffix.size_;
		return this->subview({ '[', -end_size, -1, ']' }) == suffix;
	}

	[[nodiscard]] constexpr codeunit_sequence_view remove_prefix(const codeunit_sequence_view& prefix) const noexcept
	{
		return this->starts_with(prefix) ? this->subview({ '[', prefix.size_, '~' }) : *this;
	}

	[[nodiscard]] constexpr codeunit_sequence_view remove_suffix(const codeunit_sequence_view& suffix) const noexcept
	{
		return this->ends_with(suffix) ? this->subview({ '[', 0, -suffix.size_, ')' }) : *this;
	}

	[[nodiscard]] constexpr codeunit_sequence_view trim_start(const codeunit_sequence_view& units = codeunit_sequence_view(" \t")) const noexcept
	{
		if(this->is_empty())
			return { };
		for(i32 i = 0; i < this->size_; ++i)
			if(const char codeunit = this->data_[i]; !units.contains(codeunit))
				return this->subview({ '[', i, '~' });
		return { };
	}

	[[nodiscard]] constexpr codeunit_sequence_view trim_end(const codeunit_sequence_view& units = codeunit_sequence_view(" \t")) const noexcept
	{
		if(this->is_empty())
			return { };

		for(i32 i = this->size_ - 1; i >= 0; --i)
			if(const char codeunit = this->data_[i]; !units.contains(codeunit))
				return this->subview({ '[', 0, i, ']' });
		return { };
	}

	[[nodiscard]] constexpr codeunit_sequence_view trim(const codeunit_sequence_view& units = codeunit_sequence_view(" \t")) const noexcept
	{
		return this->trim_start(units).trim_end(units);
	}

	[[nodiscard]] constexpr u32 get_hash() const noexcept
	{
		return helper::math::hash::hash_crc32(this->data_, this->size_);
	}

private:

	i32 size_ = 0;
	const char* data_ = nullptr;

};

NS_EASY_END

inline namespace literal
{
	[[nodiscard]] constexpr easy::codeunit_sequence_view operator""_cuqv(const char* str, const size_t len) noexcept
	{
		return { str, len };
	}
}

