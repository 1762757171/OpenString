// OpenString - formatting library
//
// Copyright (c) 2022 - present, [Hoshizora Ming]
// All rights reserved.

#pragma once
#include "common/definitions.h"
#include "common/index_interval.h"
#include "common/basic_types.h"
#include "text.h"
#include <array>
#include <cmath>
#include <stdexcept>
#include <charconv>

NS_EASY_BEGIN

class format_error final : public std::runtime_error
{
public:
    explicit format_error(const codeunit_sequence_view& message)
        : std::runtime_error(message.c_str()) 
    { }

    template<class Arg1, class...Args>
    format_error(const codeunit_sequence_view& message_format, Arg1&& arg1, Args&&...args);
};

template<class T>
struct formatter
{
    static codeunit_sequence format_argument(const T& value, const codeunit_sequence_view& specification)
    {
        return { };
    }
};

namespace details
{
    class format_view
    {
    public:
    #pragma region constructors

        explicit constexpr format_view(const codeunit_sequence_view& format) noexcept
            : format_(format)
        { }

        explicit constexpr format_view(const text_view& format) noexcept
            : format_(format.data())
        { }

    #pragma endregion constructors

    #pragma region iterators

        enum class run_type : u8
        {
            plain_text,
            escaped_brace,
            formatter,
            ending
        };

        struct const_iterator
        {
            constexpr const_iterator() noexcept = default;

            explicit constexpr const_iterator(const codeunit_sequence_view& v) noexcept
                : format(v)
            { }

            constexpr const_iterator& initialize() noexcept
            {
                const auto [ first_type, first_range ] = this->parse_run(0);
                this->type = first_type;
                this->range = first_range;
                return *this;
            }

            [[nodiscard]] constexpr std::tuple<run_type, codeunit_sequence_view> operator*() const noexcept
            {
                return { this->type, this->format.subview(this->range) };
            }

            constexpr const_iterator& operator++()
            {
                if(this->range.is_infinity())
                {
                    this->type = run_type::ending;
                    this->range = index_interval::empty();
                    return *this;
                }
                const auto [ next_type, next_range ] = this->parse_run(this->range.get_exclusive_max());
                this->type = next_type;
                this->range = next_range;
                return *this;
            }

            constexpr const_iterator operator++(int) noexcept
            {
                const const_iterator tmp = *this;
                ++*this;
                return tmp;
            }

            [[nodiscard]] constexpr bool operator==(const const_iterator& rhs) const noexcept
            {
                return this->type == rhs.type && this->range == rhs.range;
            }

            [[nodiscard]] constexpr bool operator!=(const const_iterator& rhs) const noexcept
            {
                return !(*this == rhs);
            }

            [[nodiscard]] constexpr std::tuple<run_type, index_interval> parse_run(const i32 from) const
            {
                const i32 index = this->format.index_any_of("{}"_cuqv, { '[', from, '~' });
                if(index == index_invalid)
                    return { run_type::plain_text, { '[', from, '~' } };
                if(from != index)
                    return { run_type::plain_text, { '[', from, index, ')' } };
                if(format[index] == format[index+1])
                    return { run_type::escaped_brace, { '[', index, index + 1, ']' } };
                if(format[index] == '}')
                    throw format_error("Unclosed right brace is not allowed!"_cuqv);
                const i32 index_next = this->format.index_any_of("{}"_cuqv, { '(', index, '~' });
                if(index_next == index_invalid || format[index_next] == '{')
                    throw format_error("Unclosed left brace is not allowed!"_cuqv);
                return { run_type::formatter, { '[', index, index_next, ']' } };
            }
            
            codeunit_sequence_view format;
            run_type type = run_type::ending;
            index_interval range = index_interval::empty();
        };
        
        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return const_iterator{ this->format_ }.initialize();
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return const_iterator{ this->format_ };
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
        
    private:
        codeunit_sequence_view format_;
    };

    template <class T>
    codeunit_sequence formatter_intermediate(const void* value, const codeunit_sequence_view specification)
    {
        return formatter<T>::format_argument(*static_cast<const T*>(value), specification);
    }

    struct format_argument_pack
    {
        using formatter_type = codeunit_sequence(*)(const void* value, const codeunit_sequence_view specification);

        const void* value;
        formatter_type formatter;

        template<class T>
        explicit constexpr format_argument_pack(const T& v) noexcept
            : value(static_cast<const void*>(&v))
            , formatter(formatter_intermediate<T>)
        { }

        [[nodiscard]] codeunit_sequence format(const codeunit_sequence_view specification) const
        {
            return this->formatter(this->value, specification);
        }
    };

    template<class...Args>
    codeunit_sequence format(const format_view& format, const Args&...args)
    {
        constexpr i32 argument_count = sizeof...(Args);
        const std::array<format_argument_pack, argument_count> arguments {{ format_argument_pack{ args } ... }};

        constexpr i32 manual_index = -1;
        i32 next_index = 0;
        codeunit_sequence result;
        for(const auto [ type, run ] : format)
        {
            switch (type) 
            {
            case format_view::run_type::plain_text:
                result += run;
                break;
            case format_view::run_type::escaped_brace:
                result += run[0];
                break;
            case format_view::run_type::formatter:
            {    
                const auto [ index_run, specification ] = run.subview({ '[', 1, -2, ']' }).split(":"_cuqv);
                i32 current_index = next_index;
                if (index_run.is_empty())
                {
                    if(next_index == manual_index)
                        throw format_error("Manual index is not allowed mixing with automatic index!"_cuqv);
                    ++next_index;
                }
                else
                {
                    if(next_index > 0)
                        throw format_error("Automatic index is not allowed mixing with manual index!"_cuqv);
                    const auto [ last, error ] = std::from_chars(index_run.data(), index_run.last(), current_index);
                    if(last != index_run.last())
                        throw format_error("Invalid format index [{}]!"_cuqv, index_run);
                    next_index = manual_index;
                }
                if(current_index < 0)
                    throw format_error("Invalid format index [{}]: Index should not be negative!"_cuqv, current_index);
                if(current_index >= argument_count)
                    throw format_error("Invalid format index [{}]: Index should be less than count of argument [{}]!"_cuqv, current_index, argument_count);
                result += arguments[current_index].format(specification);
            }
                break;
            case format_view::run_type::ending:
                // Unreachable
                break;
            }
        }
        return result;
    }
}

template<class...Args>
[[nodiscard]] codeunit_sequence format(const codeunit_sequence_view& format_literal, Args&&...args)
{
    return details::format(details::format_view{ format_literal }, args...);
}

template<class...Args>
[[nodiscard]] text format(const text_view& format_literal, Args&&...args)
{
    return { details::format(details::format_view{ format_literal }, args...) };
}

#pragma region formatter specializations for built-in types

namespace details
{
    codeunit_sequence format_integer(const i32& value, const codeunit_sequence_view& specification);
    codeunit_sequence format_float(const float& value, const codeunit_sequence_view& specification);
}

template<> 
struct formatter<i32>
{
    static codeunit_sequence format_argument(const i32& value, const codeunit_sequence_view& specification)
    {
        return details::format_integer(value, specification);
    }
};

template<> 
struct formatter<float>
{
    static codeunit_sequence format_argument(const float& value, const codeunit_sequence_view& specification)
    {
        return details::format_float(value, specification);
    }
};

template<> 
struct formatter<codeunit_sequence_view>
{
    static codeunit_sequence format_argument(const codeunit_sequence_view& value, const codeunit_sequence_view& specification)
    {
        return codeunit_sequence{ value };
    }
};

#pragma endregion formatter specializations for built-in types

template<class Arg1, class...Args>
format_error::format_error(const codeunit_sequence_view& message_format, Arg1&& arg1, Args&&...args)
    : std::runtime_error(format(message_format, arg1, args...).c_str())
{ }

NS_EASY_END
