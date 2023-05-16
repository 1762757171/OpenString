#include "pch.h"
#include "text_view.h"

using namespace easy;

TEST(codeunit_sequence_view, subview)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("sequence view"_cuqv);
		constexpr codeunit_sequence_view subview = view.subview({ '[', 4, 10, ')'});
		EXPECT_EQ(subview, OSTR_UTF8("ence v"_cuqv));
	}
	{
		constexpr auto view = OSTR_UTF8("你好❤a𪚥"_cuqv);
		constexpr codeunit_sequence_view subview = view[{ '[', 2, 13, ')'}];
		EXPECT_EQ(subview, OSTR_UTF8("\xA0好❤a\xF0\xAA\x9A"_cuqv));
	}
}

TEST(codeunit_sequence_view, index_of)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("This is a long string"_cuqv);
		constexpr i32 index_1 = view.index_of(OSTR_UTF8(" "_cuqv));
		EXPECT_EQ(index_1, 4);
	}
	{
		constexpr auto view = OSTR_UTF8("Hello world!"_cuqv);
		constexpr i32 index = view.index_of(OSTR_UTF8("?"_cuqv));
		EXPECT_EQ(index, index_invalid);
	}
	{
		constexpr codeunit_sequence_view view(OSTR_UTF8("long long ago long"));
		EXPECT_EQ(view.index_of(OSTR_UTF8("long"_cuqv)), 0);
		EXPECT_EQ(view.index_of(OSTR_UTF8("long"_cuqv), {'[', 3, '~'}), 5);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("long"_cuqv)), 14);
	}
	{
		constexpr codeunit_sequence_view view(OSTR_UTF8("你好❤a𪚥"));
		EXPECT_EQ(view.index_of(OSTR_UTF8("𪚥"_cuqv)), 10);
		EXPECT_EQ(view.index_of(OSTR_UTF8("\xAA"_cuqv)), 11);
		EXPECT_EQ(view.index_of(OSTR_UTF8("\xA5"_cuqv)), 4);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("\xA5"_cuqv)), 13);
	}
}

TEST(codeunit_sequence_view, split)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("This is a long string"_cuqv);
		constexpr auto subviews = view.split(OSTR_UTF8(" "_cuqv));
		constexpr std::tuple answer { OSTR_UTF8("This"_cuqv), OSTR_UTF8("is a long string"_cuqv) };
		EXPECT_EQ(subviews, answer);
	}
	{
		constexpr auto view = OSTR_UTF8("This is  a long string"_cuqv);
		constexpr auto subviews = view.split(OSTR_UTF8("  "_cuqv));
		constexpr std::tuple answer { OSTR_UTF8("This is"_cuqv), OSTR_UTF8("a long string"_cuqv) };
		EXPECT_EQ(subviews, answer);
	}
	{
		constexpr auto view = OSTR_UTF8("Hello world!"_cuqv);
		constexpr auto subviews = view.split(OSTR_UTF8("?"_cuqv));
		constexpr std::tuple answer { OSTR_UTF8("Hello world!"_cuqv), OSTR_UTF8(""_cuqv) };
		EXPECT_EQ(subviews, answer);
	}
}

TEST(codeunit_sequence_view, count)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("This is a long string "_cuqv);
		EXPECT_EQ(view.count(OSTR_UTF8(" "_cuqv)), 5);
	}
	{
		constexpr auto view = OSTR_UTF8("aaaaaaaaa"_cuqv);
		EXPECT_EQ(view.count(OSTR_UTF8("aa"_cuqv)), 4);
	}
}

TEST(codeunit_sequence_view, iterate)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		static constexpr auto sequence = OSTR_UTF8("sequence view");
		constexpr codeunit_sequence_view view(sequence);
		u32 index = 0;
		for(auto u : view)
		{
			EXPECT_EQ(u, sequence[index]);
			++index;
		}
	}
	{
		static constexpr auto sequence = OSTR_UTF8("你好❤a𪚥");
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
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr codeunit_sequence_view view_default_ctor;
		EXPECT_EQ(view_default_ctor, OSTR_UTF8(""));
	}

	{
		constexpr codeunit_sequence_view view(OSTR_UTF8("a"));
		EXPECT_EQ(view, OSTR_UTF8("a"));
	}

	{
		constexpr auto view = OSTR_UTF8("你好"_cuqv);
		EXPECT_EQ(view, OSTR_UTF8("你好"));
	}

	{
		constexpr auto view = OSTR_UTF8("😀"_cuqv);
		EXPECT_EQ(view, OSTR_UTF8("😀"));
	}
}

TEST(codeunit_sequence_view, trim)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr codeunit_sequence_view view(OSTR_UTF8("   123 1234  \t  "));
		
		constexpr codeunit_sequence_view view_trim_start = view.trim_start();
		EXPECT_EQ(view_trim_start, OSTR_UTF8("123 1234  \t  "_cuqv));

		constexpr codeunit_sequence_view view_trim_end = view.trim_end();
		EXPECT_EQ(view_trim_end, OSTR_UTF8("   123 1234"_cuqv));
		
		constexpr codeunit_sequence_view view_trim = view.trim();
		EXPECT_EQ(view_trim, OSTR_UTF8("123 1234"_cuqv));
	}

	{
		constexpr auto view = OSTR_UTF8("   你 好 😙  \t "_cuqv);
		EXPECT_EQ(view.trim_start(), OSTR_UTF8("你 好 😙  \t "_cuqv));
		EXPECT_EQ(view.trim_end(), OSTR_UTF8("   你 好 😙"_cuqv));
		EXPECT_EQ(view.trim(), OSTR_UTF8("你 好 😙"_cuqv));
	}

	{
		constexpr auto view = OSTR_UTF8("你 好 😙"_cuqv);
		EXPECT_EQ(view.trim_start(OSTR_UTF8("你 😙"_cuqv)), OSTR_UTF8("好 😙"_cuqv));
		EXPECT_EQ(view.trim_end(OSTR_UTF8("你 😙"_cuqv)), OSTR_UTF8("你 \xE5\xA5"_cuqv));
		EXPECT_EQ(view.trim(OSTR_UTF8("你 😙"_cuqv)), OSTR_UTF8("\xE5\xA5"_cuqv));
	}
	
	{
		constexpr auto view = OSTR_UTF8("   \t    "_cuqv);
		EXPECT_EQ(view.trim_start(), OSTR_UTF8(""_cuqv));
		EXPECT_TRUE(view.trim_start().is_empty());
		
		EXPECT_EQ(view.trim_end(), OSTR_UTF8(""_cuqv));
		
		EXPECT_EQ(view.trim(), OSTR_UTF8(""_cuqv));
	}
	
	{
		constexpr codeunit_sequence_view view;
		EXPECT_EQ(view, OSTR_UTF8(""_cuqv));
		EXPECT_TRUE(view.is_empty());
		EXPECT_EQ(view.trim_start(), OSTR_UTF8(""_cuqv));
		EXPECT_TRUE(view.trim_start().is_empty());
		EXPECT_EQ(view.trim_end(), OSTR_UTF8(""_cuqv));
		EXPECT_EQ(view.trim(), OSTR_UTF8(""_cuqv));
	}
}

TEST(codeunit_sequence_view, starts_ends_with)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr codeunit_sequence_view view(OSTR_UTF8("   123 1234  \t  "));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("  "_cuqv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("   "_cuqv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("   1"_cuqv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8(""_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("  \t  "_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("\t  "_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("  "_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8(""_cuqv)));
		EXPECT_FALSE(view.ends_with(OSTR_UTF8("\t"_cuqv)));
	}

	{
		constexpr auto view = OSTR_UTF8("你好😙"_cuqv);
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("\xE4"_cuqv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("你"_cuqv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("你好"_cuqv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("你好😙"_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("\x99"_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("😙"_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("好😙"_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("你好😙"_cuqv)));
	}
	
	{
		constexpr codeunit_sequence_view view;
		EXPECT_TRUE(view.starts_with(OSTR_UTF8(""_cuqv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8(""_cuqv)));
	}
}