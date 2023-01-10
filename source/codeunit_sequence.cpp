#include "helpers.h"
#include "codeunit_sequence.h"
#include "codeunit_sequence_view.h"
#include "index_interval.h"
#include "types.h"
#include <algorithm>

NS_EASY_BEGIN

#pragma region iterators
codeunit_sequence::iterator::iterator() noexcept
	: value()
{ }

codeunit_sequence::iterator::iterator(char* v) noexcept
	: value(v)
{ }

char& codeunit_sequence::iterator::operator*() const noexcept
{
	return *this->value;
}

char* codeunit_sequence::iterator::data() const noexcept
{
	return this->value;
}

std::ptrdiff_t codeunit_sequence::iterator::operator-(const iterator& rhs) const noexcept
{
	return this->value - rhs.value;
}

codeunit_sequence::iterator& codeunit_sequence::iterator::operator+=(const std::ptrdiff_t diff) noexcept
{
	this->value += diff;
	return *this;
}

codeunit_sequence::iterator& codeunit_sequence::iterator::operator-=(const std::ptrdiff_t diff) noexcept
{
	return this->operator+=(-diff);
}

codeunit_sequence::iterator codeunit_sequence::iterator::operator+(const std::ptrdiff_t diff) const noexcept
{
	iterator tmp = *this;
	tmp += diff;
	return tmp;
}

codeunit_sequence::iterator codeunit_sequence::iterator::operator-(const std::ptrdiff_t diff) const noexcept
{
	iterator tmp = *this;
	tmp -= diff;
	return tmp;
}

codeunit_sequence::iterator& codeunit_sequence::iterator::operator++() noexcept
{
	++this->value;
	return *this;
}

codeunit_sequence::iterator codeunit_sequence::iterator::operator++(int) noexcept
{
	const iterator tmp = *this;
	++*this;
	return tmp;
}

codeunit_sequence::iterator& codeunit_sequence::iterator::operator--() noexcept
{
	--this->value;
	return *this;
}

codeunit_sequence::iterator codeunit_sequence::iterator::operator--(int) noexcept
{
	const iterator tmp = *this;
	--*this;
	return tmp;
}

bool codeunit_sequence::iterator::operator==(const iterator& rhs) const noexcept
{
	return this->value == rhs.value;
}

bool codeunit_sequence::iterator::operator!=(const iterator& rhs) const noexcept
{
	return !(*this == rhs);
}

bool codeunit_sequence::iterator::operator<(const iterator& rhs) const noexcept
{
	return this->value < rhs.value;
}

bool codeunit_sequence::iterator::operator>(const iterator& rhs) const noexcept
{
	return rhs < *this;
}

bool codeunit_sequence::iterator::operator<=(const iterator& rhs) const noexcept
{
	return rhs >= *this;
}

bool codeunit_sequence::iterator::operator>=(const iterator& rhs) const noexcept
{
	return !(*this < rhs);
}

codeunit_sequence::iterator codeunit_sequence::begin() noexcept
{
	return iterator(this->data());
}

codeunit_sequence::const_iterator codeunit_sequence::begin() const noexcept
{
	return this->view().begin();
}

codeunit_sequence::iterator codeunit_sequence::end() noexcept
{
	return this->begin() + this->size();
}

codeunit_sequence::const_iterator codeunit_sequence::end() const noexcept
{
	return this->view().end();
}

codeunit_sequence::const_iterator codeunit_sequence::cbegin() const noexcept
{
	return this->begin();
}

codeunit_sequence::const_iterator codeunit_sequence::cend() const noexcept
{
	return this->end();
}

#pragma endregion iterators

codeunit_sequence::codeunit_sequence() noexcept
	: store_()
{ }

codeunit_sequence::codeunit_sequence(const i32 size) noexcept
	: store_()
{
	if(size > SSO_SIZE_MAX)
	{
		const i32 memory_capacity = helper::math::get_capacity(size + 1);
		this->as_norm().alloc = true;
		this->as_norm().size = 0;
		this->as_norm().data = new char[memory_capacity];
		this->as_norm().capacity = memory_capacity - 1;
	}
}

codeunit_sequence::codeunit_sequence(const codeunit_sequence& other) noexcept
	: codeunit_sequence(other.view())
{ }

codeunit_sequence::codeunit_sequence(codeunit_sequence&& other) noexcept
	: store_(other.store_)
{ }

codeunit_sequence& codeunit_sequence::operator=(const codeunit_sequence& other) noexcept
{
	this->operator=(codeunit_sequence(other.view()));
	return *this;
}

codeunit_sequence& codeunit_sequence::operator=(codeunit_sequence&& other) noexcept
{
	if(!this->is_short())
		delete[] this->as_norm().data;
	this->store_ = std::forward<std::array<u8, 16>>(other.store_);
	return *this;
}

codeunit_sequence::~codeunit_sequence() noexcept
{
	if(!this->is_short())
		delete[] this->as_norm().data;
}

codeunit_sequence::codeunit_sequence(const char* data) noexcept
	: codeunit_sequence(codeunit_sequence_view(data))
{ }

codeunit_sequence::codeunit_sequence(const char* from, const char* last) noexcept
	: codeunit_sequence(codeunit_sequence_view(from, last))
{ }

codeunit_sequence::codeunit_sequence(const char* data, const i32 count) noexcept
	: codeunit_sequence(codeunit_sequence_view(data, count))
{ }

codeunit_sequence::codeunit_sequence(const codeunit_sequence_view sv) noexcept
	: codeunit_sequence(sv.size())
{
	std::copy(sv.data(), sv.last(), this->data());
	const i32 size = sv.size();
	this->data()[size] = '\0';
	this->set_size(size);
}

i32 codeunit_sequence::size() const noexcept
{
	if (this->is_short())
		return this->as_sso().size;
	return this->as_norm().size;
}

codeunit_sequence_view codeunit_sequence::view() const& noexcept
{
	return { this->c_str(), this->size() };
}

bool codeunit_sequence::is_empty() const noexcept
{
	return this->size() == 0;
}

bool codeunit_sequence::operator==(codeunit_sequence_view rhs) const noexcept
{
	return this->view() == rhs;
}

bool codeunit_sequence::operator==(const codeunit_sequence& rhs) const noexcept
{
	return this->view() == rhs.view();
}

bool codeunit_sequence::operator==(const char* rhs) const noexcept
{
	return this->view() == codeunit_sequence_view(rhs);
}

bool codeunit_sequence::operator!=(codeunit_sequence_view rhs) const noexcept
{
	return this->view() != rhs;
}

bool codeunit_sequence::operator!=(const codeunit_sequence& rhs) const noexcept
{
	return this->view() != rhs.view();
}

bool codeunit_sequence::operator!=(const char* rhs) const noexcept
{
	return this->view() != codeunit_sequence_view(rhs);
}

codeunit_sequence& codeunit_sequence::operator+=(codeunit_sequence_view rhs) noexcept
{
	const i32 answer_size = this->size() + rhs.size();
	this->reserve(answer_size);
	std::copy(rhs.data(), rhs.last(), this->last());
	this->data()[answer_size] = '\0';
	this->set_size(answer_size);
	return *this;
}

codeunit_sequence& codeunit_sequence::append(const i32 count, const char codeunit) noexcept
{
	const i32 old_size = this->size();
	const i32 answer_size = old_size + count;
	this->reserve(answer_size);
	if(codeunit != '\0')
		memset(this->data() + old_size, codeunit, count);
	this->data()[answer_size] = '\0';
	this->set_size(answer_size);
	return *this;
}

codeunit_sequence& codeunit_sequence::operator+=(const codeunit_sequence& rhs) noexcept
{
	return this->operator+=(rhs.view());
}

codeunit_sequence& codeunit_sequence::operator+=(const char* rhs) noexcept
{
	return this->operator+=(codeunit_sequence_view(rhs));
}

codeunit_sequence& codeunit_sequence::operator+=(const char codeunit) noexcept
{
	*this += codeunit_sequence_view(&codeunit, 1);
	return *this;
}

codeunit_sequence codeunit_sequence::operator+(codeunit_sequence_view rhs) const noexcept
{
	codeunit_sequence str = *this;
	str += rhs;
	return str;
}

codeunit_sequence codeunit_sequence::operator+(const codeunit_sequence& rhs) const noexcept
{
	return this->operator+(rhs.view());
}

codeunit_sequence_view codeunit_sequence::subview(const index_interval& range) const noexcept
{
	return this->view().subview(range);
}

codeunit_sequence& codeunit_sequence::subsequence(const index_interval& range) noexcept
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
	const i32 from = selection.get_inclusive_min();
	if(from != 0)
	{
		const i32 last = selection.get_exclusive_max();
		std::move(this->data() + from, this->data() + last, this->data());
	}
	const i32 post_size = selection.size();
	this->data()[post_size] = '\0';
	this->set_size(post_size);

	return *this;
}

codeunit_sequence& codeunit_sequence::replace(const codeunit_sequence_view& source, const codeunit_sequence_view& destination, const index_interval& range)
{
	if(source.is_empty())
		return *this;
	const index_interval selection = range.select(this->size());
	const codeunit_sequence_view view = this->subview(selection);
	const i32 count = view.count(source);
	if(count == 0)
		return *this;
	const i32 old_size = this->size();
	const i32 src_size = source.size();
	const i32 dest_size = destination.size();
	const i32 per_delta = dest_size - src_size;
	const i32 whole_delta = per_delta * count;
	const i32 answer_size = old_size + whole_delta;

	if(per_delta == 0)
	{
		index_interval search_range = selection;
		while(true)
		{
			const i32 index = this->index_of(source, search_range);
			if(index == index_invalid)
				break;
			std::copy(destination.data(), destination.last(), this->data() + index);
			search_range = selection.intersect({ '[', index + dest_size, '~' });
		}
	}
	else if(per_delta < 0)
	{
		i32 found_index = this->index_of(source, selection);
		i32 offset = 0;
		for(i32 i = found_index; i <= answer_size; ++i)
		{
			while(i + offset == found_index)
			{
				std::copy(destination.data(), destination.last(), this->data() + i);
				i += dest_size;
				offset -= per_delta;
				const index_interval next_range = selection.intersect({ '[', i + offset, '~' });
				found_index = this->index_of(source, next_range);
			}
			this->data()[i] = this->data()[i + offset];
		}
		this->set_size(answer_size);
	}
	else
	{
		if(const i32 old_capacity = this->get_capacity(); old_capacity < answer_size)
		{
			// Need re-allocation, replace when moving
			const i32 memory_capacity = helper::math::get_capacity(answer_size + 1);
			const auto data = new char[memory_capacity];
			
			i32 found_index = this->index_of(source, selection);
			i32 offset = 0;
			for(i32 i = 0; i <= answer_size; ++i)
			{
				while(i + offset == found_index)
				{
					std::copy(destination.data(), destination.last(), data + i);
					i += dest_size;
					offset -= per_delta;
					const index_interval next_range = selection.intersect({ '[', i + offset, '~' });
					found_index = this->index_of(source, next_range);
				}
				data[i] = this->data()[i + offset];
			}
			
			if(!this->is_short())
				delete[] this->as_norm().data;
			
			this->as_norm().alloc = true;
			this->as_norm().size = answer_size;
			this->as_norm().capacity = memory_capacity - 1;
			this->as_norm().data = data;
		}
		else
		{
			// No re-allocation, replace in place from backward
			
			i32 found_index = this->last_index_of(source, selection) + src_size - 1;
			i32 offset = whole_delta;
			for(i32 i = answer_size; i >= 0; --i)
			{
				while(i - offset == found_index)
				{
					i -= dest_size;
					std::copy(destination.data(), destination.last(), this->data() + i + 1);
					offset -= per_delta;
					if(offset != 0)
					{
						const index_interval next_range = selection.intersect({ '[', 0, i - offset, ']' });
						found_index = this->last_index_of(source, next_range) + src_size - 1;
					}
				}
				this->data()[i] = this->data()[i - offset];
			}
			
			this->set_size(answer_size);
		}
	}
	
	return *this;
}

codeunit_sequence& codeunit_sequence::replace(const index_interval& range, const codeunit_sequence_view& destination)
{
	const i32 self_size = this->size();
	const index_interval selection = range.select(self_size);
	if(selection.is_empty())
		// Invalid range
		return *this;
	const i32 delta = destination.size() - selection.size();
	const i32 answer_size = self_size + delta;
	if(delta <= 0)
	{
		// Simply assignment
		for(i32 i = 0; i < destination.size(); ++i)
			this->data()[selection.get_inclusive_min() + i] = destination.data()[i];
		if (delta != 0)
		{
			const index_interval source_suffix = index_interval { '[', selection.get_exclusive_max(), '~' }.select(self_size);
			for(const i32 i : source_suffix)
				this->data()[i + delta] = this->data()[i];
			this->data()[answer_size] = '\0';
			this->set_size(answer_size);
		}
	}
	else
	{
		// TODO
	}

	return *this;
}

i32 codeunit_sequence::index_of(const codeunit_sequence_view& pattern, const index_interval& range) const noexcept
{
	return this->view().index_of(pattern, range);
}

i32 codeunit_sequence::last_index_of(const codeunit_sequence_view& pattern, const index_interval& range) const noexcept
{
	return this->view().last_index_of(pattern, range);
}

i32 codeunit_sequence::count(const codeunit_sequence_view& pattern) const noexcept
{
	return this->view().count(pattern);
}

bool codeunit_sequence::starts_with(const codeunit_sequence_view& pattern) const noexcept
{
	return this->view().starts_with(pattern);
}

bool codeunit_sequence::ends_with(const codeunit_sequence_view& pattern) const noexcept
{
	return this->view().ends_with(pattern);
}

void codeunit_sequence::empty()
{
	this->set_size(0);
	this->data()[0] = '\0';
}

void codeunit_sequence::empty(const i32 size)
{
	if(size <= this->get_capacity())
	{
		this->empty();
	}
	else
	{
		if(!this->is_short())
			delete[] this->as_norm().data;
		const i32 memory_capacity = helper::math::get_capacity(size + 1);
		this->as_norm().alloc = true;
		this->as_norm().size = 0;
		this->as_norm().data = new char[memory_capacity];
		this->as_norm().data[0] = '\0';
		this->as_norm().capacity = memory_capacity - 1;
	}
}

void codeunit_sequence::reserve(const i32 size)
{
	if(size <= this->get_capacity())
		return;
	const i32 memory_capacity = helper::math::get_capacity(size + 1);
	const auto data = new char[memory_capacity];
	const i32 old_size = this->size();
	std::copy(this->data(), this->last(), data);
	if(!this->is_short())
		delete[] this->as_norm().data;
	this->as_norm().alloc = true;
	this->as_norm().size = old_size;
	this->as_norm().capacity = memory_capacity - 1;
	this->as_norm().data = data;
}

void codeunit_sequence::write_at(const i32 index, const char codeunit) noexcept
{
	this->data()[index + (index >= 0 ? 0 : this->size())] = codeunit;
}

const char& codeunit_sequence::read_at(const i32 index) const noexcept
{
	return this->view().read_at(index);
}

char& codeunit_sequence::operator[](const i32 index) noexcept
{
	return this->data()[index + (index >= 0 ? 0 : this->size())];
}

const char& codeunit_sequence::operator[](const i32 index) const noexcept
{
	return this->read_at(index);
}

codeunit_sequence& codeunit_sequence::reverse(const index_interval& range) noexcept
{
	const i32 self_size = this->size();
	const index_interval selection = range.select(self_size);
	const i32 size = selection.size();
	for(i32 i = 0; i < size / 2; ++i)
	{
		char& forward = this->data()[selection.get_inclusive_min() + i];
		char& backward = this->data()[selection.get_inclusive_max() - i];
		const char temp = forward;
		forward = backward;
		backward = temp;
	}
	return *this;
}

[[nodiscard]] std::vector<codeunit_sequence_view> codeunit_sequence::split(const codeunit_sequence_view& splitter, bool cull_empty) const noexcept
{
	std::vector<codeunit_sequence_view> pieces;
	this->split(splitter, pieces, cull_empty);
	return pieces;
}

u32 codeunit_sequence::split(const codeunit_sequence_view& splitter, std::vector<codeunit_sequence_view>& pieces, bool cull_empty) const noexcept
{
	codeunit_sequence_view view = this->view();
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

codeunit_sequence& codeunit_sequence::self_trim_start(const codeunit_sequence_view& characters) noexcept
{
	if(this->is_empty())
		return *this;
	const i32 size = this->size();
	for(i32 i = 0; i < size; ++i)
		if(!characters.contains(this->view()[i]))
			return this->subsequence({ '[', i, '~' });
	this->empty();
	return *this;
}

codeunit_sequence& codeunit_sequence::self_trim_end(const codeunit_sequence_view& characters) noexcept
{
	if(this->is_empty())
		return *this;
	const i32 size = this->size();
	for(i32 i = size - 1; i >= 0; --i)
		if(!characters.contains(this->view()[i]))
			return this->subsequence({ '[', 0, i, ']' });
	this->empty();
	return *this;
}

codeunit_sequence& codeunit_sequence::self_trim(const codeunit_sequence_view& characters) noexcept
{
	// trim_start involves memory copy, but trim_end do not
	// so trim_end first may reduce copy
	return this->self_trim_end(characters).self_trim_start(characters);
}

codeunit_sequence_view codeunit_sequence::view_trim_start(const codeunit_sequence_view& characters) const noexcept
{
	return this->view().trim_start(characters);
}

codeunit_sequence_view codeunit_sequence::view_trim_end(const codeunit_sequence_view& characters) const noexcept
{
	return this->view().trim_end(characters);
}

codeunit_sequence_view codeunit_sequence::view_trim(const codeunit_sequence_view& characters) const noexcept
{
	return this->view().trim(characters);
}

uint32_t codeunit_sequence::get_hash() const noexcept
{
	return this->view().get_hash();
}

const char* codeunit_sequence::c_str() const noexcept
{
	return this->is_short() ? this->as_sso().data.data() : this->as_norm().data;
}

bool codeunit_sequence::is_short_size(const i32 size) noexcept
{
	return size <= SSO_SIZE_MAX;
}

codeunit_sequence::sso& codeunit_sequence::as_sso()
{
	return reinterpret_cast<sso&>(this->store_);
}

const codeunit_sequence::sso& codeunit_sequence::as_sso() const
{
	return reinterpret_cast<const sso&>(this->store_);
}

codeunit_sequence::norm& codeunit_sequence::as_norm()
{
	return reinterpret_cast<norm&>(this->store_);
}

const codeunit_sequence::norm& codeunit_sequence::as_norm() const
{
	return reinterpret_cast<const norm&>(this->store_);
}

bool codeunit_sequence::is_short() const
{
	return !this->as_sso().alloc;
}

i32 codeunit_sequence::get_capacity() const
{
	return this->is_short() ? SSO_SIZE_MAX : this->as_norm().capacity;
}

char* codeunit_sequence::data()
{
	return this->is_short() ? this->as_sso().data.data() : this->as_norm().data;
}

const char* codeunit_sequence::data() const
{
	return this->is_short() ? this->as_sso().data.data() : this->as_norm().data;
}

char* codeunit_sequence::last()
{
	return this->data() + this->size();
}

const char* codeunit_sequence::last() const
{
	return this->data() + this->size();
}

void codeunit_sequence::set_size(i32 size)
{
	if(this->is_short()) this->as_sso().size = static_cast<u8>(size); else this->as_norm().size = size;
}

bool operator==(const codeunit_sequence_view& lhs, const codeunit_sequence& rhs) noexcept
{
	return rhs == lhs;
}

NS_EASY_END
