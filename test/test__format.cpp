
#include "pch.h"

#include "format.h"

#include <limits>

using namespace ostr;

#define EXPECT_CHECKED_WITH_MESSAGE(statement, expected_message)

TEST(format, built_in_types)
{
    SCOPED_DETECT_MEMORY_LEAK()
    
    // escaped braces
    EXPECT_EQ("{}"_cuqv, format("{{}}"_cuqv));

    // integer
    EXPECT_EQ("0"_cuqv, format("{}"_cuqv, 0));
    EXPECT_EQ("00255"_txtv, format("{:05d}"_txtv, 255));
    EXPECT_EQ("ff"_cuqv, format("{:x}"_cuqv, 255));
    EXPECT_EQ("-0xff"_cuqv, format("{:#x}"_cuqv, -255));
    EXPECT_EQ("_1762757171"_cuqv, format("_{}"_cuqv, 1762757171ull));

    // float
    EXPECT_EQ("3.14"_cuqv, format("{}"_cuqv, 3.14f));
    EXPECT_EQ("3.1"_cuqv, format("{:.1f}"_cuqv, 3.14f));
    EXPECT_EQ("-3.14000"_cuqv, format("{:.5f}"_cuqv, -3.14f));
    EXPECT_EQ("-100"_cuqv, format("{}"_cuqv, -99.999999999));
    EXPECT_EQ("60.004"_cuqv, format("{}"_cuqv, 60.004));
    EXPECT_EQ("inf"_cuqv, format("{}"_cuqv, std::numeric_limits<f32>::infinity()));
    EXPECT_EQ("-inf"_txtv, format("{}"_txtv, -std::numeric_limits<f64>::infinity()));
    EXPECT_EQ("nan"_cuqv, format("{}"_cuqv, std::numeric_limits<f32>::quiet_NaN()));
    EXPECT_CHECKED_WITH_MESSAGE(format("{:.10f}"_cuqv, 3.14f), "Too high precision for float type [10]!");

    // pointer
    EXPECT_EQ("nullptr"_cuqv, format("{}"_cuqv, nullptr));
    EXPECT_EQ("0x00000000075bcd15"_cuqv, format("{}"_cuqv, reinterpret_cast<void *>(123456789)));

    // string
    EXPECT_EQ("繁星明 😀"_cuqv, format("{}{}明{}"_cuqv, "繁", "星"_cuqv, codeunit_sequence(" 😀")));
}

TEST(format, undefined_type)
{
    SCOPED_DETECT_MEMORY_LEAK()
    
    struct test_struct
    {
        int a;
        int b;
    };
    [[maybe_unused]] constexpr test_struct ts1 { 123456, 654321 };
    EXPECT_CHECKED_WITH_MESSAGE(format("{}"_cuqv, ts1), "Undefined format with raw memory bytes: 40 e2 01 00 f1 fb 09 00!");
    constexpr test_struct ts2 { 17627, 57171 };
    EXPECT_EQ(format("{:r}"_cuqv, ts2), "db 44 00 00 53 df 00 00"_cuqv);
}

TEST(format, manual_index)
{
    SCOPED_DETECT_MEMORY_LEAK()
    
    EXPECT_EQ("My name is 繁星明 and I'm 25 years old."_cuqv, format("My name is {1} and I'm {0} years old."_cuqv, 25, "繁星明"_cuqv));
}

TEST(format, format_exception)
{
    SCOPED_DETECT_MEMORY_LEAK()
    
    EXPECT_CHECKED_WITH_MESSAGE(format("{}{ {}"_cuqv, 3), "Unclosed left brace is not allowed!");
    EXPECT_CHECKED_WITH_MESSAGE(format("{}} "_cuqv, 3), "Unclosed right brace is not allowed!");
    EXPECT_CHECKED_WITH_MESSAGE(format("{}{}"_cuqv, 3), "Invalid format index [1]: Index should be less than count of argument [1]!");
    EXPECT_CHECKED_WITH_MESSAGE(format("{}{}}"_cuqv, 3, 4), "Unclosed right brace is not allowed!");
    EXPECT_CHECKED_WITH_MESSAGE(format("{} {0}"_cuqv, 3), "Automatic index is not allowed mixing with manual index!");
    EXPECT_CHECKED_WITH_MESSAGE(format("{0} {}"_cuqv, 3), "Manual index is not allowed mixing with automatic index!");

    EXPECT_CHECKED_WITH_MESSAGE(format("{abc}"_cuqv, 123), "Invalid format index [abc]!");      // named argument is not allowed.
    EXPECT_CHECKED_WITH_MESSAGE(format("{:.1fa}"_cuqv, 3.14f), "Invalid format specification [.1fa]!");
}
