
#include "pch.h"
#include "format.h"
#include <limits>

using namespace easy;

#define EXPECT_THROW_WITH_MESSAGE(statement, expected_exception, expected_message) \
EXPECT_THROW({ try { \
    (void)(statement); \
} catch (const expected_exception &e) { \
    EXPECT_STREQ(expected_message, e.what()); \
    throw; \
} }, expected_exception)

TEST(format, built_in_types)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    // escaped braces
    EXPECT_EQ("{}"_cuqv, format("{{}}"_cuqv));

    // integer
    EXPECT_EQ("0"_cuqv, format("{}"_cuqv, 0));
    EXPECT_EQ("00255"_txtv, format("{:05d}"_txtv, 255));
    EXPECT_EQ("ff"_cuqv, format("{:x}"_cuqv, 255));
    EXPECT_EQ("-0xff"_cuqv, format("{:#x}"_cuqv, -255));

    // float
    EXPECT_EQ("3.14"_cuqv, format("{}"_cuqv, 3.14f));
    EXPECT_EQ("3.1"_cuqv, format("{:.1f}"_cuqv, 3.14f));
    EXPECT_EQ("3.14000"_cuqv, format("{:.5f}"_cuqv, 3.14f));
    EXPECT_EQ("inf"_cuqv, format("{}"_cuqv, std::numeric_limits<float>::infinity()));
    EXPECT_EQ("-inf"_txtv, format("{}"_txtv, -std::numeric_limits<float>::infinity()));
    EXPECT_EQ("nan"_cuqv, format("{}"_cuqv, std::numeric_limits<float>::quiet_NaN()));
    EXPECT_THROW_WITH_MESSAGE(format("{:.10f}"_cuqv, 3.14f), format_error, "Too high precision for float [10]!");

    // index interval
    EXPECT_EQ("∅"_txtv, format("{}", index_interval::empty()));
    EXPECT_EQ("(-∞,20)"_txtv, format("{}", index_interval{ '~', 20, ')' }));
    EXPECT_EQ("(-∞,+∞)"_txtv, format("{}", index_interval::all()));

    // pointer
    EXPECT_EQ("nullptr"_cuqv, format("{}"_cuqv, nullptr));
    EXPECT_EQ("0x00000000075bcd15"_cuqv, format("{}"_cuqv, reinterpret_cast<void *>(123456789)));

    // string
    EXPECT_EQ("繁星明 😀"_cuqv, format("{}{}明{}"_cuqv, "繁", "星"_cuqv, codeunit_sequence(" 😀")));
}

TEST(format, undefined_type)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    struct test_struct
    {
        int a;
        int b;
    };
    constexpr test_struct ts1 { 123456, 654321 };
    EXPECT_THROW_WITH_MESSAGE(format("{}"_cuqv, ts1), format_error, "Undefined format with raw memory bytes: 40 e2 01 00 f1 fb 09 00!");
    constexpr test_struct ts2 { 17627, 57171 };
    EXPECT_EQ(format("{:r}"_cuqv, ts2), "[Undefined type (raw: db 44 00 00 53 df 00 00)]"_cuqv);
}

TEST(format, manual_index)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    EXPECT_EQ("My name is 繁星明 and I'm 25 years old."_cuqv, format("My name is {1} and I'm {0} years old."_cuqv, 25, "繁星明"_cuqv));
}

TEST(format, format_exception)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    EXPECT_THROW_WITH_MESSAGE(format("{}{ {}"_cuqv, 3), format_error, "Unclosed left brace is not allowed!");
    EXPECT_THROW_WITH_MESSAGE(format("{}} "_cuqv, 3), format_error, "Unclosed right brace is not allowed!");
    EXPECT_THROW_WITH_MESSAGE(format("{-2}"_cuqv, 3), format_error, "Invalid format index [-2]: Index should not be negative!");
    EXPECT_THROW_WITH_MESSAGE(format("{}{}"_cuqv, 3), format_error, "Invalid format index [1]: Index should be less than count of argument [1]!");
    EXPECT_THROW_WITH_MESSAGE(format("{}{}}"_cuqv, 3, 4), format_error, "Unclosed right brace is not allowed!");
    EXPECT_THROW_WITH_MESSAGE(format("{} {0}"_cuqv, 3), format_error, "Automatic index is not allowed mixing with manual index!");
    EXPECT_THROW_WITH_MESSAGE(format("{0} {}"_cuqv, 3), format_error, "Manual index is not allowed mixing with automatic index!");

    EXPECT_THROW_WITH_MESSAGE(format("{abc}"_cuqv, 123), format_error, "Invalid format index [abc]!");      // named argument is not allowed.
    EXPECT_THROW_WITH_MESSAGE(format("{:.1fa}"_cuqv, 3.14f), format_error, "Invalid format specification [.1fa]!");
}
