
#include "pch.h"

#include "text.h"

using namespace ostr;

TEST(text, construct)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		const text t;
		EXPECT_TRUE(t.is_empty());
	}
	{
		const text t("Hello 🌏!"_cuqv);
		EXPECT_FALSE(t.is_empty());
		EXPECT_EQ(t.size(), 8);
	}
	{
		const text t = "This is a very long text.";
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
	SCOPED_DETECT_MEMORY_LEAK()
	{
		const text result = text::build("Hello ", "World!"_txtv);
		EXPECT_EQ(result, "Hello World!");
	}
	{
		const text txt_ai("😘");

		const text combine = text::build("我", " very "_txtv, txt_ai, "あなた"_txtv, "!");
		const text correct("我 very 😘あなた!");

		EXPECT_EQ(combine, correct);
	}
}

TEST(text, join)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		const sequence<text_view> views = { "This"_txtv, "is"_txtv, "a"_txtv, "very"_txtv, "very"_txtv, "long"_txtv, "text"_txtv };
		const text joined_1 = text::join(views, "/**/"_txtv);
		EXPECT_EQ(joined_1, "This/**/is/**/a/**/very/**/very/**/long/**/text"_txtv);
		const text joined_2 = text::join(views, ""_txtv);
		EXPECT_EQ(joined_2, "Thisisaveryverylongtext"_txtv);
	}
}

TEST(text, reverse)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		text t("你好a😙😙你");
		t.reverse();
		EXPECT_EQ(t, "你😙😙a好你"_txtv);
		t.reverse(1, 3);
		EXPECT_EQ(t, "你a😙😙好你"_txtv);
	}
}

TEST(text, replace)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		text t("你好😙😙你");
		t.write_at(2, 'a'_cp);
		EXPECT_EQ(t, "你好a😙你"_txtv);
	}
	{
		const text t("0123456789");
		u64 n = 0;
		for(const auto cp : t)
		{
			EXPECT_EQ(cp.size(), 1);
			EXPECT_EQ(n, cp.get_codepoint() - U'0');
			++n;
		}
	}
	{
		text t("0123456789");
		u64 n = 0;
		for(auto cp : t)
		{
			if(n == 6)
			{
				cp = "你好😀"_txtv;
				break;
			}
			++n;
		}
		EXPECT_EQ(t, "012345你好😀789"_txtv);
	}
}

TEST(text, remove_prefix_suffix)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		text t = "She says: 你好😙"_txtv;
		t.self_remove_prefix("He says:");
		EXPECT_EQ(t, "She says: 你好😙"_txtv);
		t.self_remove_prefix("She says: ");
		EXPECT_EQ(t, "你好😙"_txtv);
		t.self_remove_suffix("😙"_txtv);
		EXPECT_EQ(t, "你好"_txtv);
	}
}

TEST(text, trim)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		text t("");
		EXPECT_TRUE(t.self_trim_start().is_empty());
	}
	{
		text t("");
		EXPECT_TRUE(t.self_trim_end().is_empty());
	}
	{
		text t("");
		EXPECT_EQ(t.self_trim(), ""_txtv);
	}
	
	{
		text t("   \t    ");
		EXPECT_TRUE(t.self_trim_start().is_empty());
	}
	{
		text t("   \t    ");
		EXPECT_TRUE(t.self_trim_end().is_empty());
	}
	{
		text t("   \t    ");
		EXPECT_EQ(t.self_trim(), ""_txtv);
	}
	
	{
		text t("   123 1234    ");
		EXPECT_EQ(t.self_trim_start(), "123 1234    "_txtv);
	}
	{
		text t("   123 1234    ");
		EXPECT_EQ(t.self_trim_end(), "   123 1234"_txtv);
	}
	{
		text t("   123 1234    ");
		EXPECT_EQ(t.self_trim(), "123 1234"_txtv);
	}

	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim_start(), "你 好 😙  \t "_txtv);
	}
	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim_end(), "   你 好 😙"_txtv);
	}
	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim(), "你 好 😙"_txtv);
	}

	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim_start("你 😙"_txtv), "好 😙  \t "_txtv);
	}
	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim_end("你 😙"_txtv), "   你 好 😙  \t"_txtv);
	}
	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim("你 😙"_txtv), "好 😙  \t"_txtv);
	}

	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim_start("你 😙\t"_txtv), "好 😙  \t "_txtv);
	}
	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim_end("你 😙\t"_txtv), "   你 好"_txtv);
	}
	{
		text t("   你 好 😙  \t "_txtv);
		EXPECT_EQ(t.self_trim("你 😙\t"_txtv), "好"_txtv);
	}
}
