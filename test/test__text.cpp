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
		const text t("Hello ğ!"_cuqv);
		EXPECT_FALSE(t.is_empty());
		EXPECT_EQ(t.size(), 8);
	}
	{
		const text t = "This is a very long text.";
		EXPECT_FALSE(t.is_empty());
		EXPECT_EQ(t.size(), 25);
	}
	{
		const text t = text::from_utf32(U"Hello ğ!");
		EXPECT_FALSE(t.is_empty());
		EXPECT_EQ(t.size(), 8);
	}
}

TEST(text, concatenate)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const text result = text::build("Hello ", "World!"_txtv);
		EXPECT_EQ(result, "Hello World!");
	}
	{
		const text txt_ai("ğ");

		const text combine = text::build("æ", " very "_txtv, txt_ai, "ããªã"_txtv, "!");
		const text correct("æ very ğããªã!");

		EXPECT_EQ(combine, correct);
	}
}

TEST(text, join)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const std::vector<text_view> views = { "This"_txtv, "is"_txtv, "a"_txtv, "very"_txtv, "very"_txtv, "long"_txtv, "text"_txtv };
		const text joined_1 = text::join(views, "/**/"_txtv);
		EXPECT_EQ(joined_1, "This/**/is/**/a/**/very/**/very/**/long/**/text"_txtv);
		const text joined_2 = text::join(views, ""_txtv);
		EXPECT_EQ(joined_2, "Thisisaveryverylongtext"_txtv);
	}
}

TEST(text, reverse)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		text t("ä½ å¥½ağğä½ ");
		t.reverse();
		EXPECT_EQ(t, "ä½ ğğaå¥½ä½ "_txtv);
		t.reverse({ '[', 1, -2, ')' });
		EXPECT_EQ(t, "ä½ ağğå¥½ä½ "_txtv);
	}
}

TEST(text, replace)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		text t("ä½ å¥½ğğä½ ");
		t.write_at(2, 'a'_cp);
		EXPECT_EQ(t, "ä½ å¥½ağä½ "_txtv);
	}
	{
		const text t("0123456789");
		i32 n = 0;
		for(const auto cp : t)
		{
			EXPECT_EQ(cp.size(), 1);
			EXPECT_EQ(n, cp.get_codepoint() - U'0');
			++n;
		}
	}
	{
		text t("0123456789");
		i32 n = 0;
		for(auto cp : t)
		{
			if(n == 6)
			{
				cp = "ä½ å¥½ğ"_txtv;
				break;
			}
			++n;
		}
		EXPECT_EQ(t, "012345ä½ å¥½ğ789"_txtv);
	}
}

TEST(text, remove_prefix_suffix)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		text t = "She says: ä½ å¥½ğ"_txtv;
		t.self_remove_prefix("He says:");
		EXPECT_EQ(t, "She says: ä½ å¥½ğ"_txtv);
		t.self_remove_prefix("She says: ");
		EXPECT_EQ(t, "ä½ å¥½ğ"_txtv);
		t.self_remove_suffix("ğ"_txtv);
		EXPECT_EQ(t, "ä½ å¥½"_txtv);
	}
}

TEST(text, trim)
{
	SCOPED_DETECT_MEMORY_LEAK
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
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim_start(), "ä½  å¥½ ğ  \t "_txtv);
	}
	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim_end(), "   ä½  å¥½ ğ"_txtv);
	}
	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim(), "ä½  å¥½ ğ"_txtv);
	}

	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim_start("ä½  ğ"_txtv), "å¥½ ğ  \t "_txtv);
	}
	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim_end("ä½  ğ"_txtv), "   ä½  å¥½ ğ  \t"_txtv);
	}
	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim("ä½  ğ"_txtv), "å¥½ ğ  \t"_txtv);
	}

	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim_start("ä½  ğ\t"_txtv), "å¥½ ğ  \t "_txtv);
	}
	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim_end("ä½  ğ\t"_txtv), "   ä½  å¥½"_txtv);
	}
	{
		text t("   ä½  å¥½ ğ  \t "_txtv);
		EXPECT_EQ(t.self_trim("ä½  ğ\t"_txtv), "å¥½"_txtv);
	}
}
