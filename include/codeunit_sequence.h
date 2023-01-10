#pragma once

#include "definitions.h"
#include <array>
#include <functional>
#include <vector>
#include "codeunit_sequence_view.h"
#include "index_interval.h"

NS_EASY_BEGIN

class EASY_EXPORT codeunit_sequence
{
public:

#pragma region constructors

	codeunit_sequence() noexcept;
	explicit codeunit_sequence(i32 size) noexcept;
	codeunit_sequence(const codeunit_sequence&) noexcept;
	codeunit_sequence(codeunit_sequence&&) noexcept;
	codeunit_sequence& operator=(const codeunit_sequence&) noexcept;
	codeunit_sequence& operator=(codeunit_sequence&&) noexcept;

	~codeunit_sequence() noexcept;

	codeunit_sequence(const char* data) noexcept;
	codeunit_sequence(const char* from, const char* last) noexcept;
	codeunit_sequence(const char* data, i32 count) noexcept;

	codeunit_sequence(codeunit_sequence_view sv) noexcept;

	template<typename Container>
	static codeunit_sequence join(const Container& container, const codeunit_sequence_view& separator) noexcept;

#pragma endregion constructors

#pragma region iterators

	using const_iterator = codeunit_sequence_view::const_iterator;

	struct iterator
	{
		iterator() noexcept;
		explicit iterator(char* v) noexcept;
		[[nodiscard]] char& operator*() const noexcept;
		[[nodiscard]] char* data() const noexcept;
		[[nodiscard]] std::ptrdiff_t operator-(const iterator& rhs) const noexcept;
		iterator& operator+=(std::ptrdiff_t diff) noexcept;
		iterator& operator-=(std::ptrdiff_t diff) noexcept;
		[[nodiscard]] iterator operator+(std::ptrdiff_t diff) const noexcept;
		[[nodiscard]] iterator operator-(std::ptrdiff_t diff) const noexcept;
	    iterator& operator++() noexcept;
	    iterator operator++(int) noexcept;
		iterator& operator--() noexcept;
		iterator operator--(int) noexcept;
	    [[nodiscard]] bool operator==(const iterator& rhs) const noexcept;
	    [[nodiscard]] bool operator!=(const iterator& rhs) const noexcept;
	    [[nodiscard]] bool operator<(const iterator& rhs) const noexcept;
	    [[nodiscard]] bool operator>(const iterator& rhs) const noexcept;
	    [[nodiscard]] bool operator<=(const iterator& rhs) const noexcept;
	    [[nodiscard]] bool operator>=(const iterator& rhs) const noexcept;
		
		char* value;
	};
	
	[[nodiscard]] iterator begin() noexcept;
	[[nodiscard]] const_iterator begin() const noexcept;
	[[nodiscard]] iterator end() noexcept;
	[[nodiscard]] const_iterator end() const noexcept;
	[[nodiscard]] const_iterator cbegin() const noexcept;
	[[nodiscard]] const_iterator cend() const noexcept;

#pragma endregion iterators

	[[nodiscard]] codeunit_sequence_view view() const& noexcept;
	/**
	 * FATAL: You must NOT get view from rvalue of a codeunit sequence
	 */
	codeunit_sequence_view view() && = delete;

	/**
	 * @return The length of this codeunit sequence
	 */
	[[nodiscard]] i32 size() const noexcept;
	
	/**
	 * @return Whether this codeunit sequence is empty or not.
	 */
	[[nodiscard]] bool is_empty() const noexcept;

	/**
	 * @param rhs Another codeunit sequence
	 * @return Whether two codeunit sequences are equal.
	 */
	[[nodiscard]] bool operator==(codeunit_sequence_view rhs) const noexcept;
	[[nodiscard]] bool operator==(const codeunit_sequence& rhs) const noexcept;
	[[nodiscard]] bool operator==(const char* rhs) const noexcept;

	/**
	 * @param rhs Another codeunit sequence
	 * @return Whether two codeunit sequences are different.
	 */
	[[nodiscard]] bool operator!=(codeunit_sequence_view rhs) const noexcept;
	[[nodiscard]] bool operator!=(const codeunit_sequence& rhs) const noexcept;
	[[nodiscard]] bool operator!=(const char* rhs) const noexcept;

	/**
	 * Append a codeunit sequence back.
	 * @return ref of this codeunit sequence.
	 * @todo Think: Is operators needed? It's costly for compiler.
	 */
	codeunit_sequence& append(i32 count, char codeunit = '\0') noexcept;

	codeunit_sequence& operator+=(codeunit_sequence_view rhs) noexcept;
	codeunit_sequence& operator+=(const codeunit_sequence& rhs) noexcept;
	codeunit_sequence& operator+=(const char* rhs) noexcept;
	codeunit_sequence& operator+=(char codeunit) noexcept;

	/**
	 * Return a new codeunit sequence which concatenate two codeunit sequences.
	 * This will cause multiple memory allocations.
	 * Consider using format if you want to concatenate multiple codeunit sequences.
	 * @param rhs Another codeunit sequence which should be appended to this
	 * @return Get a new codeunit sequence instance which append a codeunit sequence back this codeunit sequence.
	 */
	[[nodiscard]] codeunit_sequence operator+(codeunit_sequence_view rhs) const noexcept;
	[[nodiscard]] codeunit_sequence operator+(const codeunit_sequence& rhs) const noexcept;

	[[nodiscard]] codeunit_sequence_view subview(const index_interval& range) const noexcept;

	/**
	 * Make this a subsequence from specific range
	 * 
	 * Example: codeunit_sequence("codeunit_sequence").subsequence(2, 3) == "ar_";
	 * 
	 * @param range Given range
	 * @return ref of this codeunit sequence
	 */
	codeunit_sequence& subsequence(const index_interval& range) noexcept;

	/**
	 * Get the index of specific codeunit_sequence.
	 * 
	 * Example: codeunit_sequence("codeunit_sequence").index_of("ar_") == 2;
	 * 
	 * @param pattern pattern to search.
	 * @param range range to search.
	 * @return the index of where the codeunit_sequence first found
	 */
	[[nodiscard]] i32 index_of(const codeunit_sequence_view& pattern, const index_interval& range = index_interval::all()) const noexcept;
	[[nodiscard]] i32 last_index_of(const codeunit_sequence_view& pattern, const index_interval& range = index_interval::all()) const noexcept;
	
	[[nodiscard]] i32 count(const codeunit_sequence_view& pattern) const noexcept;

	[[nodiscard]] bool starts_with(const codeunit_sequence_view& pattern) const noexcept;
	[[nodiscard]] bool ends_with(const codeunit_sequence_view& pattern) const noexcept;

	/**
	 * Empty the string without reallocation.
	 */
	void empty();
	/**
	 * Empty the string with size reserved.
	 * @param size the size reserved
	 */
	void empty(i32 size);
	
	void reserve(i32 size);

	void write_at(i32 index, char codeunit) noexcept;
	[[nodiscard]] const char& read_at(i32 index) const noexcept;

	[[nodiscard]] char& operator[](i32 index) noexcept;
	[[nodiscard]] const char& operator[](i32 index) const noexcept;

	codeunit_sequence& reverse(const index_interval& range = index_interval::all()) noexcept;

	[[nodiscard]] std::vector<codeunit_sequence_view> split(const codeunit_sequence_view& splitter, bool cull_empty = true) const noexcept;
	u32 split(const codeunit_sequence_view& splitter, std::vector<codeunit_sequence_view>& pieces, bool cull_empty = true) const noexcept;

	codeunit_sequence& replace(const codeunit_sequence_view& source, const codeunit_sequence_view& destination, const index_interval& range = index_interval::all());
	codeunit_sequence& replace(const index_interval& range, const codeunit_sequence_view& destination);

	codeunit_sequence& self_remove_prefix(const codeunit_sequence_view& prefix) noexcept;
	codeunit_sequence& self_remove_suffix(const codeunit_sequence_view& suffix) noexcept;
	[[nodiscard]] codeunit_sequence_view view_remove_prefix(const codeunit_sequence_view& prefix) const noexcept;
	[[nodiscard]] codeunit_sequence_view view_remove_suffix(const codeunit_sequence_view& suffix) const noexcept;

	codeunit_sequence& self_trim_start(const codeunit_sequence_view& characters = codeunit_sequence_view(" \t")) noexcept;
	codeunit_sequence& self_trim_end(const codeunit_sequence_view& characters = codeunit_sequence_view(" \t")) noexcept;
	codeunit_sequence& self_trim(const codeunit_sequence_view& characters = codeunit_sequence_view(" \t")) noexcept;
	[[nodiscard]] codeunit_sequence_view view_trim_start(const codeunit_sequence_view& characters = codeunit_sequence_view(" \t")) const noexcept;
	[[nodiscard]] codeunit_sequence_view view_trim_end(const codeunit_sequence_view& characters = codeunit_sequence_view(" \t")) const noexcept;
	[[nodiscard]] codeunit_sequence_view view_trim(const codeunit_sequence_view& characters = codeunit_sequence_view(" \t")) const noexcept;

	[[nodiscard]] uint32_t get_hash() const noexcept;

	[[nodiscard]] const char* c_str() const noexcept;

private:

	static constexpr u8 SSO_SIZE_MAX = 14;
	static bool is_short_size(i32 size) noexcept;

	struct sso
	{
		u8 alloc : 1;
		u8 size : 7;
		std::array<char, SSO_SIZE_MAX + 1> data;
	};

	struct norm
	{
		u32 alloc : 1;
		i32 size : 15;
		i32 capacity;	// character capacity, which is 1 less than memory capacity
		char* data;
	};

	[[nodiscard]] sso& as_sso();
	[[nodiscard]] const sso& as_sso() const;

	[[nodiscard]] norm& as_norm();
	[[nodiscard]] const norm& as_norm() const;

	/// @return is this a sequence with less than 15 chars
	[[nodiscard]] bool is_short() const;

	[[nodiscard]] i32 get_capacity() const;

	[[nodiscard]] char* data();

	[[nodiscard]] const char* data() const;

	[[nodiscard]] char* last();

	[[nodiscard]] const char* last() const;

	void deallocate();

	void set_size(i32 size);

	std::array<byte, 16> store_;
};

template<typename Container>
codeunit_sequence codeunit_sequence::join(const Container& container, const codeunit_sequence_view& separator) noexcept
{
	codeunit_sequence result;
	for(const auto& element : container)
	{
		if(!result.is_empty())
			result += separator;
		result += element;
	}
	return result;
}

[[nodiscard]] bool operator==(const codeunit_sequence_view& lhs, const codeunit_sequence& rhs) noexcept;

NS_EASY_END
