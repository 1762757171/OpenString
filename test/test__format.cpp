#include "pch.h"
#include "format.h"
#include <limits>

using namespace easy;

#define EXPECT_CHECKED_WITH_MESSAGE(statement, expected_message)

TEST(format, built_in_types)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    // escaped braces
    EXPECT_EQ(OSTR_UTF8("{}"_cuqv), format(OSTR_UTF8("{{}}"_cuqv)));

    // integer
    EXPECT_EQ(OSTR_UTF8("0"_cuqv), format(OSTR_UTF8("{}"_cuqv), 0));
    EXPECT_EQ(OSTR_UTF8("00255"_txtv), format(OSTR_UTF8("{:05d}"_txtv), 255));
    EXPECT_EQ(OSTR_UTF8("ff"_cuqv), format(OSTR_UTF8("{:x}"_cuqv), 255));
    EXPECT_EQ(OSTR_UTF8("-0xff"_cuqv), format(OSTR_UTF8("{:#x}"_cuqv), -255));

    // float
    EXPECT_EQ(OSTR_UTF8("3.14"_cuqv), format(OSTR_UTF8("{}"_cuqv), 3.14f));
    EXPECT_EQ(OSTR_UTF8("3.1"_cuqv), format(OSTR_UTF8("{:.1f}"_cuqv), 3.14f));
    EXPECT_EQ(OSTR_UTF8("3.14000"_cuqv), format(OSTR_UTF8("{:.5f}"_cuqv), 3.14f));
    EXPECT_EQ(OSTR_UTF8("inf"_cuqv), format(OSTR_UTF8("{}"_cuqv), std::numeric_limits<float>::infinity()));
    EXPECT_EQ(OSTR_UTF8("-inf"_txtv), format(OSTR_UTF8("{}"_txtv), -std::numeric_limits<float>::infinity()));
    EXPECT_EQ(OSTR_UTF8("nan"_cuqv), format(OSTR_UTF8("{}"_cuqv), std::numeric_limits<float>::quiet_NaN()));
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{:.10f}"_cuqv), 3.14f), "Too high precision for float type [10]!");

    // index interval
    EXPECT_EQ(OSTR_UTF8("∅"_txtv), format(OSTR_UTF8("{}"), index_interval::empty()));
    EXPECT_EQ(OSTR_UTF8("(-∞,20)"_txtv), format(OSTR_UTF8("{}"), index_interval{ '~', 20, ')' }));
    EXPECT_EQ(OSTR_UTF8("(-∞,+∞)"_txtv), format(OSTR_UTF8("{}"), index_interval::all()));

    // pointer
    EXPECT_EQ(OSTR_UTF8("nullptr"_cuqv), format(OSTR_UTF8("{}"_cuqv), nullptr));
    EXPECT_EQ(OSTR_UTF8("0x00000000075bcd15"_cuqv), format(OSTR_UTF8("{}"_cuqv), reinterpret_cast<void *>(123456789)));

    // string
    EXPECT_EQ(OSTR_UTF8("繁星明 😀"_cuqv), format(OSTR_UTF8("{}{}明{}"_cuqv), 
        OSTR_UTF8("繁"), OSTR_UTF8("星"_cuqv), codeunit_sequence(OSTR_UTF8(" 😀"))));
}

TEST(format, undefined_type)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    struct test_struct
    {
        int a;
        int b;
    };
    [[maybe_unused]]constexpr test_struct ts1 { 123456, 654321 };
    EXPECT_CHECKED_WITH_MESSAGE(format("{}"_cuqv, ts1), "Undefined format with raw memory bytes: 40 e2 01 00 f1 fb 09 00!");
    constexpr test_struct ts2 { 17627, 57171 };
    EXPECT_EQ(format(OSTR_UTF8("{:r}"_cuqv), ts2), OSTR_UTF8("[Undefined type (raw: db 44 00 00 53 df 00 00)]"_cuqv));
}

TEST(format, manual_index)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    EXPECT_EQ(OSTR_UTF8("My name is 繁星明 and I'm 25 years old."_cuqv), 
        format(OSTR_UTF8("My name is {1} and I'm {0} years old."_cuqv), 25, OSTR_UTF8("繁星明"_cuqv)));
}

TEST(format, format_exception)
{
    SCOPED_DETECT_MEMORY_LEAK
    
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{}{ {}"_cuqv, 3)), "Unclosed left brace is not allowed!");
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{}} "_cuqv, 3)), "Unclosed right brace is not allowed!");
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{-2}"_cuqv, 3)), "Invalid format index [-2]: Index should not be negative!");
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{}{}"_cuqv, 3)), "Invalid format index [1]: Index should be less than count of argument [1]!");
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{}{}}"_cuqv, 3, 4)), "Unclosed right brace is not allowed!");
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{} {0}"_cuqv, 3)), "Automatic index is not allowed mixing with manual index!");
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{0} {}"_cuqv, 3)), "Manual index is not allowed mixing with automatic index!");

    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{abc}"_cuqv, 123)), "Invalid format index [abc]!");      // named argument is not allowed.
    EXPECT_CHECKED_WITH_MESSAGE(format(OSTR_UTF8("{:.1fa}"_cuqv, 3.14f)), "Invalid format specification [.1fa]!");
}
