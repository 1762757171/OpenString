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
    class format_error : public std::runtime_error
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
    static codeunit_sequence format_to(const T& value, const codeunit_sequence_view& specification)
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

        constexpr format_view(const codeunit_sequence_view& format) noexcept
            : format_(format)
        { }

        constexpr format_view(const text_view& format) noexcept
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
        return formatter<T>::format_to(*static_cast<const T*>(value), specification);
    }

    struct format_argument
    {
        using formatter_type = codeunit_sequence(*)(const void* value, const codeunit_sequence_view specification);

        const void* value;
        formatter_type formatter;

        template<class T>
        constexpr format_argument(const T& v) noexcept
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
        const std::array<format_argument, argument_count> arguments {{ { args } ... }};

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

    [[nodiscard]] constexpr u8 get_digit_count(const u8 size) noexcept
    {
        if(size == 1)
            return 4; // "-128"
        if(size == 2)
            return 6; // "-32768"
        if(size == 4)
            return 11; // "-2147483648"
        if(size == 8)
            return 20; // "-9223372036854775808"
        return 0;
    }
}

template<class...Args>
[[nodiscard]] codeunit_sequence format(const codeunit_sequence_view& format_literal, Args&&...args)
{
    return details::format({ format_literal }, args...);
}

template<class...Args>
[[nodiscard]] text format(const text_view& format_literal, Args&&...args)
{
    return { details::format({ format_literal }, args...) };
}

#pragma region formatter specializations for built-in types

template<> 
struct formatter<i32>
{
    static codeunit_sequence format_to(const i32& value, const codeunit_sequence_view& specification)
    {
        char type = 'd';
        i32 holding = -1;
        if (!specification.is_empty())
        {
            codeunit_sequence_view parsing = specification;
            if ("bcdox"_cuqv.contains(parsing[-1]))
            {
                type = parsing[0];
                parsing = parsing.subview({ '[', 0, -2, ']' });
            }
            if(!parsing.is_empty())
            {
                const i32 zero_index = parsing.index_of('0');
                if(zero_index != index_invalid)
                {
                    const codeunit_sequence_view holding_view = parsing.subview({ '(', zero_index, '~' });
                    const auto [ last, error ] = std::from_chars(holding_view.data(), holding_view.last(), holding);
                    if(last != holding_view.last())
                        throw format_error("Invalid format specification [{}]!"_cuqv, specification);
                    parsing = parsing.subview({ '[', 0, zero_index, ')' });
                }
            }
            if(!parsing.is_empty())
                throw format_error("Invalid format specification [{}]!"_cuqv, specification);
        }
        i32 base = 10;
        switch (type)
        {
        case 'b':
            base = 2;
            break;
        case 'c':
            return codeunit_sequence{ static_cast<char>(value) };
        case 'd':
            break;
        case 'o':
            base = 8;
            break;
        case 'x':
            base = 16;
            break;
        default:
            break;
        }
        char buffer[details::get_digit_count(sizeof(i32))];
        const auto [ last, error ] = std::to_chars(buffer, buffer + sizeof(buffer), value, base);
        const i32 used_size = static_cast<i32>(last - buffer);
        if(used_size < holding)
        {
            const i32 padding_size = holding - used_size;
            codeunit_sequence result(holding);
            for(i32 i = 0; i < padding_size; ++i)
                result += '0';
            result += codeunit_sequence_view{ buffer, last };
            return result;
        }

        return codeunit_sequence{ buffer, last };
    }
};

template<> 
struct formatter<float>
{
    static codeunit_sequence format_to(const float& value, const codeunit_sequence_view& specification)
    {
        if (isinf(value))
            return codeunit_sequence{ value < 0 ? "-inf"_cuqv : "inf"_cuqv };
        if (isnan(value))
            return codeunit_sequence{ "nan"_cuqv };
        i32 precision = -1;
        char type = 'g';
        if (!specification.is_empty())
        {
            codeunit_sequence_view parsing = specification;
            if("aefg"_cuqv.contains( parsing[-1] ))
            {    
                type = parsing[-1];
                parsing = parsing.subview({ '[', 0, -2, ']' });
            }
            if(!parsing.is_empty())
            {
                const i32 dot_index = parsing.index_of('.');
                if(dot_index != index_invalid)
                {
                    const codeunit_sequence_view precision_view = parsing.subview({ '(', dot_index, '~' });
                    const auto [ last, error ] = std::from_chars(precision_view.data(), precision_view.last(), precision);
                    if(last != precision_view.last())
                        throw format_error("Invalid format specification [{}]!"_cuqv, specification);
                    parsing = parsing.subview({ '[', 0, dot_index, ')' });
                }
            }
            if(!parsing.is_empty())
                throw format_error("Invalid format specification [{}]!"_cuqv, specification);
        }
        std::chars_format format = std::chars_format::general;
        switch (type) 
        {
        case 'a':
            format = std::chars_format::hex;
            break;
        case 'e':
            format = std::chars_format::scientific;
            break;
        case 'f':
            format = std::chars_format::fixed;
            break;
        case 'g':
            format = std::chars_format::general;
            break;
        default:
            break;
        }
        if(precision == -1)
        {    
            // float has a max length of 16 characters: "-3.402823466e+38".
            char buffer[16];
            const auto [ last, error ] = std::to_chars(buffer, buffer + sizeof(buffer), value, format);
            return codeunit_sequence{ buffer, last };
        }
        static constexpr i32 max_precision = 9;
        if(precision > max_precision)
            throw format_error("Too high precision for float [{}]!"_cuqv, precision);
        char buffer[max_precision];
        const auto [ last, error] = std::to_chars(buffer, buffer + sizeof(buffer), value, format, precision);
        return codeunit_sequence{ buffer, last };
    }
};

template<> 
struct formatter<codeunit_sequence_view>
{
    static codeunit_sequence format_to(const codeunit_sequence_view& value, const codeunit_sequence_view& specification)
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
