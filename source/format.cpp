#include "format.h"

NS_EASY_BEGIN

namespace details
{
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

codeunit_sequence details::format_integer(const i32& value, const codeunit_sequence_view& specification)
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

codeunit_sequence details::format_float(const float& value, const codeunit_sequence_view& specification)
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

NS_EASY_END
