#include "text.h"

NS_EASY_BEGIN

text::text() noexcept = default;
text::text(nullptr_t) noexcept { }
text::text(const text&) noexcept = default;
text::text(text&&) noexcept = default;
text& text::operator=(const text&) noexcept = default;
text& text::operator=(text&&) noexcept = default;
text::~text() = default;

text::text(const char* str) noexcept
	: sequence_(str)
{ }

text::text(const text_view view) noexcept
	: sequence_(view.data())
{ }

text::text(codeunit_sequence sequence) noexcept
	: sequence_(std::move(sequence))
{ }

text::text(codeunit_sequence_view sequence) noexcept
	: sequence_(sequence)
{ }

text text::from_utf8(const char* str) noexcept
{
	return { codeunit_sequence_view(str) };
}

text::const_iterator text::begin() const noexcept
{
	return this->view().begin();
}

text::const_iterator text::end() const noexcept
{
	return this->view().end();
}

text::const_iterator text::cbegin() const noexcept
{
	return this->view().cbegin();
}

text::const_iterator text::cend() const noexcept
{
	return this->view().cend();
}

text_view text::view() const noexcept
{
	return text_view(this->sequence_);
}

i32 text::size() const noexcept
{
	return this->view().size();
}

bool text::is_empty() const noexcept
{
	return this->sequence_.is_empty();
}

bool text::operator==(const text_view& rhs) const noexcept
{
	return this->view() == rhs;
}

bool text::operator==(const text& rhs) const noexcept
{
	return this->view() == rhs.view();
}

bool text::operator==(const char* rhs) const noexcept
{
	return this->view() == rhs;
}

bool text::operator!=(const text_view& rhs) const noexcept
{
	return this->view() != rhs;
}

bool text::operator!=(const text& rhs) const noexcept
{
	return this->view() != rhs.view();
}

bool text::operator!=(const char* rhs) const noexcept
{
	return this->view() != rhs;
}

text& text::operator+=(const text_view& rhs) noexcept
{
	this->sequence_ += rhs.data();
	return *this;
}

text& text::operator+=(const text& rhs) noexcept
{
	return this->operator+=(rhs.view());
}

text_view text::subview(const index_interval& range) const noexcept
{
	return this->view().subview(range);
}

text& text::subtext(const index_interval& range) noexcept
{
	const i32 self_size = this->size();
	const index_interval selection = range.select(self_size);
	if(selection.is_empty())
	{
		this->empty();
		return *this;
	}
	if(selection == index_interval::from_universal(self_size))
		// Do nothing
		return *this;
	const i32 lower_bound = this->view().get_codepoint_index( selection.get_inclusive_min() );
	const i32 upper_bound = this->view().get_codepoint_index( selection.get_exclusive_max() );
	this->sequence_.subsequence({ '[', lower_bound, upper_bound, ')' });
	return *this;
}

i32 text::index_of(const text_view& pattern, const index_interval& range) const noexcept
{
	return this->view().index_of(pattern, range);
}

i32 text::last_index_of(const text_view& pattern, const index_interval& range) const noexcept
{
	return this->view().last_index_of(pattern, range);
}

i32 text::count(const text_view& pattern) const noexcept
{
	return this->view().count(pattern);
}

bool text::starts_with(const text_view& prefix) const noexcept
{
	return this->view().starts_with(prefix);
}

bool text::ends_with(const text_view& suffix) const noexcept
{
	return this->view().ends_with(suffix);
}

void text::empty() noexcept
{
	this->sequence_.empty();
}

void text::write_at(const i32 index, const codepoint cp) noexcept
{
	this->replace({ '[', index, index, ']' }, text_view(cp));
}

codepoint text::read_at(const i32 index) const noexcept
{
	return this->view().read_at(index);
}

codepoint text::operator[](const i32 index) const noexcept
{
	return this->view()[index];
}

text& text::reverse(const index_interval& range) noexcept
{
	const index_interval sequence_range = this->view().get_codeunit_range(range);
	this->sequence_.reverse(sequence_range);
	i32 lower_bound = sequence_range.get_inclusive_min();
	for(const i32 i : sequence_range)
	{
		if(unicode::parse_utf8_length( this->sequence_[i] ) != 0)
		{
			this->sequence_.reverse({ '[', lower_bound, i, ']' });
			lower_bound = i + 1;
		}
	}
	return *this;
}

[[nodiscard]] std::vector<text_view> text::split(const text_view& splitter, const bool cull_empty) const noexcept
{
	std::vector<text_view> pieces;
	this->split(splitter, pieces, cull_empty);
	return pieces;
}

u32 text::split(const text_view& splitter, std::vector<text_view>& pieces, const bool cull_empty) const noexcept
{
	text_view view = this->view();
	u32 count = 0;
	while(true)
	{
		const auto [ left, right ] = view.split(splitter);
		if(!cull_empty || !left.is_empty())
			pieces.push_back(left);
		++count;
		if(right.is_empty())
			break;
		view = right;
	}
	return count;
}

text& text::replace(const text_view& source, const text_view& destination, const index_interval& range)
{
	const index_interval codeunit_range = this->view().get_codeunit_range(range);
	this->sequence_.replace(source.data(), destination.data(), codeunit_range);
	return *this;
}

text& text::replace(const index_interval& range, const text_view& destination)
{
	const index_interval codeunit_range = this->view().get_codeunit_range(range);
	this->sequence_.replace(codeunit_range, destination.data());
	return *this;
}

text& text::self_trim_start(const text_view& characters) noexcept
{
	if(this->is_empty())
		return *this;
	i32 codeunit_index = 0;
	for(auto it = this->cbegin(); it != this->cend(); ++it)
	{
		if(!characters.contains(it.get_codepoint()))
			break;
		codeunit_index += it.size();
	}
	this->sequence_.subsequence({ '[', codeunit_index, '~' });
	return *this;
}

text& text::self_trim_end(const text_view& characters) noexcept
{
	if(this->is_empty())
		return *this;
	i32 codeunit_index = 0;
	auto it = this->cend();
	while(true)
	{
		--it;
		if(!characters.contains(it.get_codepoint()))
			break;
		codeunit_index -= it.size();
		if(it == this->cbegin())
			break;
	}
	this->sequence_.subsequence({ '[', 0, codeunit_index, ')' });
	return *this;
}

text& text::self_trim(const text_view& characters) noexcept
{
	return this->self_trim_end(characters).self_trim_start(characters);
}

text_view text::view_trim_start(const text_view& characters) const noexcept
{
	return this->view().trim_start(characters);
}

text_view text::view_trim_end(const text_view& characters) const noexcept
{
	return this->view().trim_end(characters);
}

text_view text::view_trim(const text_view& characters) const noexcept
{
	return this->view().trim(characters);
}

uint32_t text::get_hash() const noexcept
{
	return this->view().get_hash();
}

const char* text::c_str() const noexcept
{
	return this->view().c_str();
}

bool operator==(const text_view& lhs, const text& rhs) noexcept
{
	return rhs == lhs;
}

NS_EASY_END


