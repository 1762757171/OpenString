
#include "pch.h"

#include "text_view.h"

using namespace ostr;

TEST(text_view, iterate)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		constexpr auto view = "你 好 😙"_txtv;
		constexpr u64 size = view.size();
		EXPECT_EQ(size, 5);
		constexpr codepoint cp[] = { U'你'_cp, U' '_cp, U'好'_cp, U' '_cp, U'😙'_cp };
		u64 index = 0;
		for(const auto c : view)
		{
			EXPECT_EQ(c, cp[index]);
			++index;
		}
	}
}

TEST(text_view, subtext)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		constexpr auto view = "text view"_txtv;
		constexpr text_view subview = view.subview(3, 4);
		EXPECT_EQ(subview, "t vi"_txtv);
	}
	{
		constexpr auto view = "你好❤a𪚥"_txtv;
		constexpr text_view subview = view.subview(1, 3);
		EXPECT_EQ(subview, "好❤a"_txtv);
	}
}

TEST(text_view, trim)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		constexpr text_view view("   123 1234    ");
		
		constexpr text_view view_trim_start = view.trim_start();
		EXPECT_EQ(view_trim_start, "123 1234    "_txtv);
		
		constexpr text_view view_trim_end = view.trim_end();
		EXPECT_EQ(view_trim_end, "   123 1234"_txtv);
		
		constexpr text_view view_trim = view.trim();
		EXPECT_EQ(view_trim, "123 1234"_txtv);
	}

	{
		constexpr auto view = "   你 好 😙  \t "_txtv;
		EXPECT_EQ(view.trim_start(), "你 好 😙  \t "_txtv);
		EXPECT_EQ(view.trim_end(), "   你 好 😙"_txtv);
		EXPECT_EQ(view.trim(), "你 好 😙"_txtv);
	}

	{
		constexpr text_view view("你 好 😙"_txtv);
		EXPECT_EQ(view.trim_start("你 😙"_txtv), "好 😙"_txtv);
		EXPECT_EQ(view.trim_end("你 😙"_txtv), "你 好"_txtv);
		EXPECT_EQ(view.trim("你 😙"_txtv), "好"_txtv);
	}
	
	{
		constexpr auto view = "   \t    "_txtv;
		EXPECT_EQ(view.trim_start(), ""_txtv);
		EXPECT_TRUE(view.trim_start().is_empty());
		EXPECT_EQ(view.trim_end(), ""_txtv);
		EXPECT_EQ(view.trim(), ""_txtv);
	}
	
	{
		constexpr text_view view;
		EXPECT_EQ(view, ""_txtv);
		EXPECT_TRUE(view.is_empty());
		EXPECT_EQ(view.trim_start(), ""_txtv);
		EXPECT_TRUE(view.trim_start().is_empty());
		EXPECT_EQ(view.trim_end(), ""_txtv);
		EXPECT_EQ(view.trim(), ""_txtv);
	}
}

TEST(text_view, index_of)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		constexpr auto view = "你❤好❤a𪚥"_txtv;
		EXPECT_EQ(view.index_of("❤"_txtv), 1);
		EXPECT_EQ(view.index_of("a"_txtv), 4);
		EXPECT_EQ(view.last_index_of("𪚥"_txtv), 5);
		EXPECT_EQ(view.last_index_of("❤"_txtv), 3);
	}
	{
		const text_view view("This is a very long text.");
		EXPECT_EQ(view.index_of("is"_txtv), 2);
		EXPECT_EQ(view.index_of("is"_txtv, 3, 5), 5);
		EXPECT_EQ(view.index_of("is"_txtv, 6), global_constant::INDEX_INVALID);
		EXPECT_EQ(view.last_index_of("is"_txtv), 5);
		EXPECT_EQ(view.last_index_of("is"_txtv, 0, 4), 2);
	}
	{
		const text_view view("你好😙😙你");
		EXPECT_EQ(view.index_of("你"_txtv), 0);
		EXPECT_EQ(view.index_of(U'你'_cp, 1), 4);
		EXPECT_EQ(view.index_of("你"_txtv, 1, 3), global_constant::INDEX_INVALID);
		EXPECT_EQ(view.index_of("😙"_txtv), 2);
		EXPECT_EQ(view.last_index_of("😙"_txtv), 3);
		EXPECT_EQ(view.last_index_of("😙"_txtv, 0, 3), 2);
	}
}

TEST(text_view, access)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		constexpr auto view = "你❤好❤a𪚥"_txtv;
		EXPECT_EQ(view[3], U'❤');
	}
}

TEST(text_view, starts_ends_with)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		constexpr text_view view("   123 1234  \t  ");
		EXPECT_TRUE(view.starts_with("  "_txtv));
		EXPECT_TRUE(view.starts_with("   "_txtv));
		EXPECT_TRUE(view.starts_with("   1"_txtv));
		EXPECT_TRUE(view.starts_with(""_txtv));
		EXPECT_TRUE(view.ends_with("  \t  "_txtv));
		EXPECT_TRUE(view.ends_with("\t  "_txtv));
		EXPECT_TRUE(view.ends_with("  "_txtv));
		EXPECT_TRUE(view.ends_with(""_txtv));
		EXPECT_FALSE(view.ends_with("\t"_txtv));
	}

	{
		constexpr auto view = "你好😙"_txtv;
		EXPECT_TRUE(view.starts_with("你"_txtv));
		EXPECT_TRUE(view.starts_with("你好"_txtv));
		EXPECT_TRUE(view.starts_with("你好😙"_txtv));
		EXPECT_TRUE(view.ends_with("😙"_txtv));
		EXPECT_TRUE(view.ends_with("好😙"_txtv));
		EXPECT_TRUE(view.ends_with("你好😙"_txtv));
	}
	
	{
		constexpr text_view view;
		EXPECT_TRUE(view.starts_with(""_txtv));
		EXPECT_TRUE(view.ends_with(""_txtv));
	}
}
