#include "pch.h"
#include "text_view.h"

using namespace easy;

TEST(text_view, iterate)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("你 好 😙"_txtv);
		constexpr int size = view.size();
		EXPECT_EQ(size, 5);
		constexpr codepoint cp[] = { U'你'_cp, U' '_cp, U'好'_cp, U' '_cp, U'😙'_cp };
		i32 index = 0;
		for(const auto c : view)
		{
			EXPECT_EQ(c, cp[index]);
			++index;
		}
	}
}

TEST(text_view, subtext)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("text view"_txtv);
		constexpr text_view subview = view.subview({ '(', 2, 6,']' });
		EXPECT_EQ(subview, OSTR_UTF8("t vi"_txtv));
	}
	{
		constexpr auto view = OSTR_UTF8("你好❤a𪚥"_txtv);
		constexpr text_view subview = view[{'[', 1, 4,')'}];
		EXPECT_EQ(subview, OSTR_UTF8("好❤a"_txtv));
	}
}

TEST(text_view, trim)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr text_view view(OSTR_UTF8("   123 1234    "));
		
		constexpr text_view view_trim_start = view.trim_start();
		EXPECT_EQ(view_trim_start, OSTR_UTF8("123 1234    "_txtv));
		
		constexpr text_view view_trim_end = view.trim_end();
		EXPECT_EQ(view_trim_end, OSTR_UTF8("   123 1234"_txtv));
		
		constexpr text_view view_trim = view.trim();
		EXPECT_EQ(view_trim, OSTR_UTF8("123 1234"_txtv));
	}

	{
		constexpr auto view = OSTR_UTF8("   你 好 😙  \t "_txtv);
		EXPECT_EQ(view.trim_start(), OSTR_UTF8("你 好 😙  \t "_txtv));
		EXPECT_EQ(view.trim_end(), OSTR_UTF8("   你 好 😙"_txtv));
		EXPECT_EQ(view.trim(), OSTR_UTF8("你 好 😙"_txtv));
	}

	{
		constexpr text_view view(OSTR_UTF8("你 好 😙"_txtv));
		EXPECT_EQ(view.trim_start(OSTR_UTF8("你 😙"_txtv)), OSTR_UTF8("好 😙"_txtv));
		EXPECT_EQ(view.trim_end(OSTR_UTF8("你 😙"_txtv)), OSTR_UTF8("你 好"_txtv));
		EXPECT_EQ(view.trim(OSTR_UTF8("你 😙"_txtv)), OSTR_UTF8("好"_txtv));
	}
	
	{
		constexpr auto view = OSTR_UTF8("   \t    "_txtv);
		EXPECT_EQ(view.trim_start(), OSTR_UTF8(""_txtv));
		EXPECT_TRUE(view.trim_start().is_empty());
		EXPECT_EQ(view.trim_end(), OSTR_UTF8(""_txtv));
		EXPECT_EQ(view.trim(), OSTR_UTF8(""_txtv));
	}
	
	{
		constexpr text_view view;
		EXPECT_EQ(view, OSTR_UTF8(""_txtv));
		EXPECT_TRUE(view.is_empty());
		EXPECT_EQ(view.trim_start(), OSTR_UTF8(""_txtv));
		EXPECT_TRUE(view.trim_start().is_empty());
		EXPECT_EQ(view.trim_end(), OSTR_UTF8(""_txtv));
		EXPECT_EQ(view.trim(), OSTR_UTF8(""_txtv));
	}
}

TEST(text_view, index_of)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("你❤好❤a𪚥"_txtv);
		EXPECT_EQ(view.index_of(OSTR_UTF8("❤"_txtv)), 1);
		EXPECT_EQ(view.index_of(OSTR_UTF8("a"_txtv)), 4);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("𪚥"_txtv)), 5);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("❤"_txtv)), 3);
	}
	{
		const text_view view(OSTR_UTF8("This is a very long text."));
		EXPECT_EQ(view.index_of(OSTR_UTF8("is"_txtv)), 2);
		EXPECT_EQ(view.index_of(OSTR_UTF8("is"_txtv), { '[', 3, 8, ')' }), 5);
		EXPECT_EQ(view.index_of(OSTR_UTF8("is"_txtv), { '[', 6, '~' }), index_invalid);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("is"_txtv)), 5);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("is"_txtv), { '[', 0, 4, ')' }), 2);
	}
	{
		const text_view view(OSTR_UTF8("你好😙😙你"));
		EXPECT_EQ(view.index_of(OSTR_UTF8("你"_txtv)), 0);
		EXPECT_EQ(view.index_of(U'你'_cp, { '[', 1, '~' }), 4);
		EXPECT_EQ(view.index_of(OSTR_UTF8("你"_txtv), { '[', 1, 3, ']' }), index_invalid);
		EXPECT_EQ(view.index_of(OSTR_UTF8("😙"_txtv)), 2);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("😙"_txtv)), 3);
		EXPECT_EQ(view.last_index_of(OSTR_UTF8("😙"_txtv), { '[', 0, 2, ']' }), 2);
	}
}

TEST(text_view, access)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr auto view = OSTR_UTF8("你❤好❤a𪚥"_txtv);
		EXPECT_EQ(view[3], U'❤');
	}
}

TEST(text_view, starts_ends_with)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		constexpr text_view view(OSTR_UTF8("   123 1234  \t  "));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("  "_txtv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("   "_txtv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("   1"_txtv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8(""_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("  \t  "_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("\t  "_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("  "_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8(""_txtv)));
		EXPECT_FALSE(view.ends_with(OSTR_UTF8("\t"_txtv)));
	}

	{
		constexpr auto view = OSTR_UTF8("你好😙"_txtv);
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("你"_txtv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("你好"_txtv)));
		EXPECT_TRUE(view.starts_with(OSTR_UTF8("你好😙"_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("😙"_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("好😙"_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8("你好😙"_txtv)));
	}
	
	{
		constexpr text_view view;
		EXPECT_TRUE(view.starts_with(OSTR_UTF8(""_txtv)));
		EXPECT_TRUE(view.ends_with(OSTR_UTF8(""_txtv)));
	}
}
