#include "pch.h"
#include "text_view.h"

using namespace easy;

TEST(codeunit_sequence_view, subview)
{
	{
		constexpr auto view = "sequence view"_cuqv;
		constexpr codeunit_sequence_view subview = view.subview({ '[', 4, 10, ')'});
		EXPECT_EQ(subview, "ence v"_cuqv);
	}
	{
		constexpr auto view = "你好❤a𪚥"_cuqv;
		constexpr codeunit_sequence_view subview = view[{ '[', 2, 13, ')'}];
		EXPECT_EQ(subview, "\xA0好❤a\xF0\xAA\x9A"_cuqv);
	}
}

TEST(codeunit_sequence_view, index_of)
{
	{
		constexpr auto view = "This is a long string"_cuqv;
		constexpr i32 index_1 = view.index_of(" "_cuqv);
		EXPECT_EQ(index_1, 4);
	}
	{
		constexpr auto view = "Hello world!"_cuqv;
		constexpr i32 index = view.index_of("?"_cuqv);
		EXPECT_EQ(index, index_invalid);
	}
	{
		constexpr codeunit_sequence_view view("long long ago long");
		EXPECT_EQ(view.index_of("long"_cuqv), 0);
		EXPECT_EQ(view.index_of("long"_cuqv, {'[', 3, '~'}), 5);
		EXPECT_EQ(view.last_index_of("long"_cuqv), 14);
	}
	{
		constexpr codeunit_sequence_view view("你好❤a𪚥");
		EXPECT_EQ(view.index_of("𪚥"_cuqv), 10);
		EXPECT_EQ(view.index_of("\xAA"_cuqv), 11);
		EXPECT_EQ(view.index_of("\xA5"_cuqv), 4);
		EXPECT_EQ(view.last_index_of("\xA5"_cuqv), 13);
	}
}

TEST(codeunit_sequence_view, split)
{
	{
		constexpr auto view = "This is a long string"_cuqv;
		constexpr auto subviews = view.split(" "_cuqv);
		constexpr std::tuple answer { "This"_cuqv, "is a long string"_cuqv };
		EXPECT_EQ(subviews, answer);
	}
	{
		constexpr auto view = "This is  a long string"_cuqv;
		constexpr auto subviews = view.split("  "_cuqv);
		constexpr std::tuple answer { "This is"_cuqv, "a long string"_cuqv };
		EXPECT_EQ(subviews, answer);
	}
	{
		constexpr auto view = "Hello world!"_cuqv;
		constexpr auto subviews = view.split("?"_cuqv);
		constexpr std::tuple answer { "Hello world!"_cuqv, ""_cuqv };
		EXPECT_EQ(subviews, answer);
	}
}

TEST(codeunit_sequence_view, count)
{
	{
		constexpr auto view = "This is a long string "_cuqv;
		EXPECT_EQ(view.count(" "_cuqv), 5);
	}
}

TEST(codeunit_sequence_view, iterate)
{
	{
		static constexpr char sequence[] = "sequence view";
		constexpr codeunit_sequence_view view(sequence);
		u32 index = 0;
		for(auto u : view)
		{
			EXPECT_EQ(u, sequence[index]);
			++index;
		}
	}
	{
		static constexpr char sequence[] = "你好❤a𪚥";
		constexpr codeunit_sequence_view view(sequence);
		constexpr codeunit_sequence_view subview = view.subview({ '[', 2, 13,')'});
		u32 index = 2;
		for(auto u : subview)
		{
			EXPECT_EQ(u, sequence[index]);
			++index;
		}
		EXPECT_EQ(index, 13);
	}
}

TEST(codeunit_sequence_view, equal)
{
	{
		constexpr codeunit_sequence_view view_default_ctor;
		EXPECT_EQ(view_default_ctor, "");
	}

	{
		constexpr codeunit_sequence_view view("a");
		EXPECT_EQ(view, "a");
	}

	{
		constexpr auto view = "你好"_cuqv;
		EXPECT_EQ(view, "你好");
	}

	{
		constexpr auto view = "😀"_cuqv;
		EXPECT_EQ(view, "😀");
	}
}

TEST(codeunit_sequence_view, trim)
{
	{
		constexpr codeunit_sequence_view view("   123 1234  \t  ");
		
		constexpr codeunit_sequence_view view_trim_start = view.trim_start();
		EXPECT_EQ(view_trim_start, "123 1234  \t  "_cuqv);

		constexpr codeunit_sequence_view view_trim_end = view.trim_end();
		EXPECT_EQ(view_trim_end, "   123 1234"_cuqv);
		
		constexpr codeunit_sequence_view view_trim = view.trim();
		EXPECT_EQ(view_trim, "123 1234"_cuqv);
	}

	{
		constexpr auto view = "   你 好 😙  \t "_cuqv;
		EXPECT_EQ(view.trim_start(), "你 好 😙  \t "_cuqv);
		EXPECT_EQ(view.trim_end(), "   你 好 😙"_cuqv);
		EXPECT_EQ(view.trim(), "你 好 😙"_cuqv);
	}

	{
		constexpr auto view = "你 好 😙"_cuqv;
		EXPECT_EQ(view.trim_start("你 😙"_cuqv), "好 😙"_cuqv);
		EXPECT_EQ(view.trim_end("你 😙"_cuqv), "你 \xE5\xA5"_cuqv);
		EXPECT_EQ(view.trim("你 😙"_cuqv), "\xE5\xA5"_cuqv);
	}
	
	{
		constexpr auto view = "   \t    "_cuqv;
		EXPECT_EQ(view.trim_start(), ""_cuqv);
		EXPECT_TRUE(view.trim_start().is_empty());
		
		EXPECT_EQ(view.trim_end(), ""_cuqv);
		
		EXPECT_EQ(view.trim(), ""_cuqv);
	}
	
	{
		constexpr codeunit_sequence_view view;
		EXPECT_EQ(view, ""_cuqv);
		EXPECT_TRUE(view.is_empty());
		EXPECT_EQ(view.trim_start(), ""_cuqv);
		EXPECT_TRUE(view.trim_start().is_empty());
		EXPECT_EQ(view.trim_end(), ""_cuqv);
		EXPECT_EQ(view.trim(), ""_cuqv);
	}
}

TEST(codeunit_sequence_view, starts_ends_with)
{
	{
		constexpr codeunit_sequence_view view("   123 1234  \t  ");
		EXPECT_TRUE(view.starts_with("  "_cuqv));
		EXPECT_TRUE(view.starts_with("   "_cuqv));
		EXPECT_TRUE(view.starts_with("   1"_cuqv));
		EXPECT_TRUE(view.starts_with(""_cuqv));
		EXPECT_TRUE(view.ends_with("  \t  "_cuqv));
		EXPECT_TRUE(view.ends_with("\t  "_cuqv));
		EXPECT_TRUE(view.ends_with("  "_cuqv));
		EXPECT_TRUE(view.ends_with(""_cuqv));
		EXPECT_FALSE(view.ends_with("\t"_cuqv));
	}

	{
		constexpr auto view = "你好😙"_cuqv;
		EXPECT_TRUE(view.starts_with("\xE4"_cuqv));
		EXPECT_TRUE(view.starts_with("你"_cuqv));
		EXPECT_TRUE(view.starts_with("你好"_cuqv));
		EXPECT_TRUE(view.starts_with("你好😙"_cuqv));
		EXPECT_TRUE(view.ends_with("\x99"_cuqv));
		EXPECT_TRUE(view.ends_with("😙"_cuqv));
		EXPECT_TRUE(view.ends_with("好😙"_cuqv));
		EXPECT_TRUE(view.ends_with("你好😙"_cuqv));
	}
	
	{
		constexpr codeunit_sequence_view view;
		EXPECT_TRUE(view.starts_with(""_cuqv));
		EXPECT_TRUE(view.ends_with(""_cuqv));
	}
}