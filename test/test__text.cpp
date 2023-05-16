#include "pch.h"
#include "text.h"

using namespace easy;

TEST(text, construct)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const text t;
		EXPECT_TRUE(t.is_empty());
	}
	{
		const text t(OSTR_UTF8("Hello 🌏!"_cuqv));
		EXPECT_FALSE(t.is_empty());
		EXPECT_EQ(t.size(), 8);
	}
	{
		const text t = OSTR_UTF8("This is a very long text.");
		EXPECT_FALSE(t.is_empty());
		EXPECT_EQ(t.size(), 25);
	}
	{
		const text t = text::from_utf32(U"Hello 🌏!");
		EXPECT_FALSE(t.is_empty());
		EXPECT_EQ(t.size(), 8);
	}
}

TEST(text, concatenate)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const text result = text::build(OSTR_UTF8("Hello "), OSTR_UTF8("World!"_txtv));
		EXPECT_EQ(result, OSTR_UTF8("Hello World!"));
	}
	{
		const text txt_ai(OSTR_UTF8("😘"));

		const text combine = text::build(OSTR_UTF8("我"), OSTR_UTF8(" very "_txtv), txt_ai, OSTR_UTF8("あなた"_txtv), OSTR_UTF8("!"));
		const text correct(OSTR_UTF8("我 very 😘あなた!"));

		EXPECT_EQ(combine, correct);
	}
}

TEST(text, join)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const std::vector<text_view> views = { 
			OSTR_UTF8("This"_txtv), OSTR_UTF8("is"_txtv), OSTR_UTF8("a"_txtv), 
			OSTR_UTF8("very"_txtv), OSTR_UTF8("very"_txtv), OSTR_UTF8("long"_txtv), OSTR_UTF8("text"_txtv) 
		};
		const text joined_1 = text::join(views, OSTR_UTF8("/**/"_txtv));
		EXPECT_EQ(joined_1, OSTR_UTF8("This/**/is/**/a/**/very/**/very/**/long/**/text"_txtv));
		const text joined_2 = text::join(views, OSTR_UTF8(""_txtv));
		EXPECT_EQ(joined_2, OSTR_UTF8("Thisisaveryverylongtext"_txtv));
	}
}

TEST(text, reverse)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		text t(OSTR_UTF8("你好a😙😙你"));
		t.reverse();
		EXPECT_EQ(t, OSTR_UTF8("你😙😙a好你"_txtv));
		t.reverse({ '[', 1, -2, ')' });
		EXPECT_EQ(t, OSTR_UTF8("你a😙😙好你"_txtv));
	}
}

TEST(text, replace)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		text t(OSTR_UTF8("你好😙😙你"));
		t.write_at(2, OSTR_UTF8('a'_cp));
		EXPECT_EQ(t, OSTR_UTF8("你好a😙你"_txtv));
	}
	{
		const text t(OSTR_UTF8("0123456789"));
		i32 n = 0;
		for(const auto cp : t)
		{
			EXPECT_EQ(cp.size(), 1);
			EXPECT_EQ(n, cp.get_codepoint() - U'0');
			++n;
		}
	}
	{
		text t(OSTR_UTF8("0123456789"));
		i32 n = 0;
		for(auto cp : t)
		{
			if(n == 6)
			{
				cp = OSTR_UTF8("你好😀"_txtv);
				break;
			}
			++n;
		}
		EXPECT_EQ(t, OSTR_UTF8("012345你好😀789"_txtv));
	}
}

TEST(text, remove_prefix_suffix)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		text t = OSTR_UTF8("She says: 你好😙"_txtv);
		t.self_remove_prefix(OSTR_UTF8("He says:"));
		EXPECT_EQ(t, OSTR_UTF8("She says: 你好😙"_txtv));
		t.self_remove_prefix(OSTR_UTF8("She says: "));
		EXPECT_EQ(t, OSTR_UTF8("你好😙"_txtv));
		t.self_remove_suffix(OSTR_UTF8("😙"_txtv));
		EXPECT_EQ(t, OSTR_UTF8("你好"_txtv));
	}
}

TEST(text, trim)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		text t(OSTR_UTF8(""));
		EXPECT_TRUE(t.self_trim_start().is_empty());
	}
	{
		text t(OSTR_UTF8(""));
		EXPECT_TRUE(t.self_trim_end().is_empty());
	}
	{
		text t(OSTR_UTF8(""));
		EXPECT_EQ(t.self_trim(), OSTR_UTF8(""_txtv));
	}
	
	{
		text t(OSTR_UTF8("   \t    "));
		EXPECT_TRUE(t.self_trim_start().is_empty());
	}
	{
		text t(OSTR_UTF8("   \t    "));
		EXPECT_TRUE(t.self_trim_end().is_empty());
	}
	{
		text t(OSTR_UTF8("   \t    "));
		EXPECT_EQ(t.self_trim(), OSTR_UTF8(""_txtv));
	}
	
	{
		text t(OSTR_UTF8("   123 1234    "));
		EXPECT_EQ(t.self_trim_start(), OSTR_UTF8("123 1234    "_txtv));
	}
	{
		text t(OSTR_UTF8("   123 1234    "));
		EXPECT_EQ(t.self_trim_end(), OSTR_UTF8("   123 1234"_txtv));
	}
	{
		text t(OSTR_UTF8("   123 1234    "));
		EXPECT_EQ(t.self_trim(), OSTR_UTF8("123 1234"_txtv));
	}

	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim_start(), OSTR_UTF8("你 好 😙  \t "_txtv));
	}
	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim_end(), OSTR_UTF8("   你 好 😙"_txtv));
	}
	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim(), OSTR_UTF8("你 好 😙"_txtv));
	}

	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim_start(OSTR_UTF8("你 😙"_txtv)), OSTR_UTF8("好 😙  \t "_txtv));
	}
	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim_end(OSTR_UTF8("你 😙"_txtv)), OSTR_UTF8("   你 好 😙  \t"_txtv));
	}
	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim(OSTR_UTF8("你 😙"_txtv)), OSTR_UTF8("好 😙  \t"_txtv));
	}

	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim_start(OSTR_UTF8("你 😙\t"_txtv)), OSTR_UTF8("好 😙  \t "_txtv));
	}
	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim_end(OSTR_UTF8("你 😙\t"_txtv)), OSTR_UTF8("   你 好"_txtv));
	}
	{
		text t(OSTR_UTF8("   你 好 😙  \t "_txtv));
		EXPECT_EQ(t.self_trim(OSTR_UTF8("你 😙\t"_txtv)), OSTR_UTF8("好"_txtv));
	}
}
