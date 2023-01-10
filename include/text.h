#pragma once
#include "codeunit_sequence.h"
#include "definitions.h"
#include "text_view.h"

NS_EASY_BEGIN

class text
{
public:

	text() noexcept;
	explicit text(nullptr_t) noexcept;		// Construct without initialization
	text(const text&) noexcept;
	text(text&&) noexcept;
	text& operator=(const text&) noexcept;
	text& operator=(text&&) noexcept;

	~text();

	// ReSharper disable CppNonExplicitConvertingConstructor
	text(const char* str) noexcept;
	text(text_view view) noexcept;
	text(codeunit_sequence sequence) noexcept;
	text(codeunit_sequence_view sequence) noexcept;
	// ReSharper restore CppNonExplicitConvertingConstructor

	static text from_utf8(const char* str) noexcept;
	static text from_utf16(const char16_t* str) noexcept;
	static text from_utf32(const char32_t* str) noexcept;
	
	template<typename Container>
	static text join(const Container& container, const text_view& separator) noexcept;

#pragma region iterator

	using const_iterator = text_view::const_iterator;

	struct iterator
	{

	};
	
	[[nodiscard]] iterator begin() noexcept;
	[[nodiscard]] const_iterator begin() const noexcept;
	[[nodiscard]] iterator end() noexcept;
	[[nodiscard]] const_iterator end() const noexcept;
	[[nodiscard]] const_iterator cbegin() const noexcept;
	[[nodiscard]] const_iterator cend() const noexcept;

#pragma endregion iterator

	[[nodiscard]] text_view view() const noexcept;

	[[nodiscard]] i32 size() const noexcept;
	[[nodiscard]] bool is_empty() const noexcept;

	[[nodiscard]] bool operator==(const text_view& rhs) const noexcept;
	[[nodiscard]] bool operator==(const text& rhs) const noexcept;
	[[nodiscard]] bool operator==(const char* rhs) const noexcept;
	[[nodiscard]] bool operator!=(const text_view& rhs) const noexcept;
	[[nodiscard]] bool operator!=(const text& rhs) const noexcept;
	[[nodiscard]] bool operator!=(const char* rhs) const noexcept;

	text& operator+=(const text_view& rhs) noexcept;
	text& operator+=(const text& rhs) noexcept;

	[[nodiscard]] text_view subview(const index_interval& range) const noexcept;

	text& subtext(const index_interval& range) noexcept;
	
	[[nodiscard]] i32 index_of(const text_view& pattern, const index_interval& range = index_interval::all()) const noexcept;
	[[nodiscard]] i32 last_index_of(const text_view& pattern, const index_interval& range = index_interval::all()) const noexcept;
	
	[[nodiscard]] i32 count(const text_view& pattern) const noexcept;

	[[nodiscard]] bool starts_with(const text_view& prefix) const noexcept;

	[[nodiscard]] bool ends_with(const text_view& suffix) const noexcept;

	/**
	 * Empty the text without reallocation.
	 */
	void empty() noexcept;

	void write_at(i32 index, codepoint cp) noexcept;
	[[nodiscard]] codepoint read_at(i32 index) const noexcept;
	
	/// @note: Please use operator[] for read-only access, or use write_at method for write access.
	[[nodiscard]] codepoint operator[](i32 index) const noexcept;

	text& reverse(const index_interval& range = index_interval::all()) noexcept;

	[[nodiscard]] std::vector<text_view> split(const text_view& splitter, bool cull_empty = true) const noexcept;
	u32 split(const text_view& splitter, std::vector<text_view>& pieces, bool cull_empty = true) const noexcept;

	text& replace(const text_view& source, const text_view& destination, const index_interval& range = index_interval::all());
	text& replace(const index_interval& range, const text_view& destination);

	text& self_trim_start(const text_view& characters = text_view(" \t")) noexcept;
	text& self_trim_end(const text_view& characters = text_view(" \t")) noexcept;
	text& self_trim(const text_view& characters = text_view(" \t")) noexcept;

	[[nodiscard]] text_view view_trim_start(const text_view& characters = text_view(" \t")) const noexcept;
	[[nodiscard]] text_view view_trim_end(const text_view& characters = text_view(" \t")) const noexcept;
	[[nodiscard]] text_view view_trim(const text_view& characters = text_view(" \t")) const noexcept;

	[[nodiscard]] uint32_t get_hash() const noexcept;

	[[nodiscard]] const char* c_str() const noexcept;

private:

	codeunit_sequence sequence_;
	
};

template<typename Container>
text text::join(const Container& container, const text_view& separator) noexcept
{
	text result;
	for (const auto it : container)
	{
		if(!result.is_empty())
			result += separator;
		result += it;
	}
	return result;
}

[[nodiscard]] bool operator==(const text_view& lhs, const text& rhs) noexcept;

NS_EASY_END
