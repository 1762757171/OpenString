// ReSharper disable StringLiteralTypo
#include "pch.h"
#include "common/index_interval.h"
#include "text.h"

using namespace easy;

// this is a struct that has same memory layout as struct codeunit_sequence
// convert the pointer of struct codeunit_sequence to this struct and access the private members.
struct codeunit_sequence_accessor
{
	static constexpr i32 SSO_SIZE_MAX = 14;

	struct sso
	{
		u8 alloc : 1;
		u8 size : 7;
		std::array<ochar8_t, SSO_SIZE_MAX + 1> data;
	};

	struct norm
	{
		u32 alloc : 1;
		i32 size : 15;
		i32 capacity;
		ochar8_t* data;
	};

	[[nodiscard]] sso& as_sso()
	{
		return reinterpret_cast<sso&>(store);
	}
	[[nodiscard]] const sso& as_sso() const
	{
		return reinterpret_cast<const sso&>(store);
	}

	[[nodiscard]] norm& as_norm()
	{
		return reinterpret_cast<norm&>(store);
	}
	[[nodiscard]] const norm& as_norm() const
	{
		return reinterpret_cast<const norm&>(store);
	}

	/// @return is this a sequence with less than 15 chars
	[[nodiscard]] bool is_short() const
	{
		return !as_sso().alloc;
	}

	std::array<ochar8_t, 16> store;
};

#define ACCESS(cuq) ((codeunit_sequence_accessor*)(&(cuq)))

TEST(codeunit_sequence, construct)
{
	SCOPED_DETECT_MEMORY_LEAK
	
	const codeunit_sequence cuq_default_ctor;
	const codeunit_sequence cuq_ctor_supplement(OSTR_UTF8("𪚥😁"));
	const codeunit_sequence cuq_ctor_long(OSTR_UTF8("👌𪚥1😁 你好"));

	EXPECT_EQ(0, cuq_default_ctor.size());
	EXPECT_EQ(8, cuq_ctor_supplement.size());
	EXPECT_EQ(20, cuq_ctor_long.size());
}

TEST(codeunit_sequence, concatenate)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const codeunit_sequence cuq_ai(OSTR_UTF8("😘"));

		const codeunit_sequence combine = codeunit_sequence::build(OSTR_UTF8("我"), cuq_ai, cuq_ai, codeunit_sequence(OSTR_UTF8("あなた")), OSTR_UTF8("!"_cuqv));
		const codeunit_sequence correct(OSTR_UTF8("我😘😘あなた!"));

		EXPECT_EQ(combine, correct);
	}
}

TEST(codeunit_sequence, split)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const codeunit_sequence sequence(OSTR_UTF8("This        is   a long string"_cuqv));
		std::vector<codeunit_sequence_view> result;
		sequence.split(OSTR_UTF8(" "_cuqv), result);
		const std::vector answer { OSTR_UTF8("This"_cuqv), OSTR_UTF8("is"_cuqv), 
			OSTR_UTF8("a"_cuqv), OSTR_UTF8("long"_cuqv), OSTR_UTF8("string"_cuqv) };
		EXPECT_EQ(result, answer);
	}
}

TEST(codeunit_sequence, trim)
{
	SCOPED_DETECT_MEMORY_LEAK
	{ 
		{
			codeunit_sequence cuq_src(OSTR_UTF8("   123 1234    "));
			cuq_src.self_trim_start();
			EXPECT_EQ(cuq_src, OSTR_UTF8("123 1234    "));
		}
		{
			codeunit_sequence cuq_src(OSTR_UTF8("   123 1234   "));
			cuq_src.self_trim_end();
			EXPECT_EQ(cuq_src, OSTR_UTF8("   123 1234"));
		}
		{
			codeunit_sequence cuq_src(OSTR_UTF8("   123 1234   "));
			cuq_src.self_trim();
			EXPECT_EQ(cuq_src, OSTR_UTF8("123 1234"));
		}
	}

	{
		{
			const codeunit_sequence cuq_origin(OSTR_UTF8("   你 好 😙  \t  "));
			codeunit_sequence cuq = cuq_origin;
			EXPECT_EQ(cuq, OSTR_UTF8("   你 好 😙  \t  "));
			cuq.self_trim_start();
			EXPECT_EQ(cuq, OSTR_UTF8("你 好 😙  \t  "));
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("   你 好 😙  \t  "));
			EXPECT_EQ(cuq.self_trim_end(), OSTR_UTF8("   你 好 😙"));
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("   你 好 😙  \t  "_cuqv));
			EXPECT_EQ(cuq.self_trim(), OSTR_UTF8("你 好 😙"));
		}
	}

	{
		codeunit_sequence cuq(OSTR_UTF8("你 好 😙"));
		EXPECT_EQ(cuq.self_trim_start(), OSTR_UTF8("你 好 😙"));
		EXPECT_EQ(cuq.self_trim_end(), OSTR_UTF8("你 好 😙"));
		EXPECT_EQ(cuq.self_trim(), OSTR_UTF8("你 好 😙"));
	}
	{
		{
			codeunit_sequence cuq(OSTR_UTF8("       "));
			EXPECT_EQ(cuq.self_trim_start(), OSTR_UTF8(""));
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("       "));
			EXPECT_EQ(cuq.self_trim_end(), OSTR_UTF8(""));
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("       "));
			EXPECT_EQ(cuq.self_trim(), OSTR_UTF8(""));
			EXPECT_TRUE(cuq.is_empty());
		}
	}
	{
		{
			codeunit_sequence cuq;
			EXPECT_EQ(cuq.self_trim_start(), OSTR_UTF8(""));
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("       "));
			EXPECT_EQ(cuq.self_trim_end(), OSTR_UTF8(""));
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("       "));
			EXPECT_EQ(cuq.self_trim(), OSTR_UTF8(""));
			EXPECT_TRUE(cuq.is_empty());
		}
	}
	{
		{
			codeunit_sequence cuq(OSTR_UTF8("你 好 😙"_cuqv));
			EXPECT_EQ(cuq.self_trim_start(OSTR_UTF8("你 😙"_cuqv)), OSTR_UTF8("好 😙"_cuqv));
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("你 好 😙"_cuqv));
			EXPECT_EQ(cuq.self_trim_end(OSTR_UTF8("你 😙"_cuqv)), OSTR_UTF8("你 \xE5\xA5"_cuqv));
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("你 好 😙"_cuqv));
			EXPECT_EQ(cuq.self_trim(OSTR_UTF8("你 😙"_cuqv)), OSTR_UTF8("\xE5\xA5"_cuqv));
		}
	}
}

TEST(codeunit_sequence, empty)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		codeunit_sequence cuq(OSTR_UTF8("12345"));
		cuq.empty(50);
		const codeunit_sequence_accessor* accessor = ACCESS(cuq);
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(0, accessor->as_norm().size);
		EXPECT_EQ(63, accessor->as_norm().capacity);
	}
	{
		codeunit_sequence cuq(OSTR_UTF8("This is a sentence with 33 words."_cuqv));
		const codeunit_sequence_accessor* accessor = ACCESS(cuq);
		cuq.empty(100);
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(0, cuq.size());
		EXPECT_EQ(127, accessor->as_norm().capacity);
		cuq.empty(20);		// Do not reallocate
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(0, accessor->as_norm().size);
		EXPECT_EQ(127, accessor->as_norm().capacity);
		cuq.empty(10);		// Do not reallocate
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(0, accessor->as_sso().size);
	}
}

TEST(codeunit_sequence, reserve)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		codeunit_sequence cuq(OSTR_UTF8("12345"));
		const codeunit_sequence_accessor* accessor = ACCESS(cuq);
		cuq.reserve(10);		// Do nothing
		EXPECT_TRUE(accessor->is_short());
		EXPECT_EQ(5, accessor->as_sso().size);
		EXPECT_EQ(OSTR_UTF8("12345"_cuqv), cuq);
		cuq.reserve(50);
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(5, accessor->as_norm().size);
		EXPECT_EQ(63, accessor->as_norm().capacity);
	}
	{
		codeunit_sequence cuq(OSTR_UTF8("This is a sentence with 33 words."_cuqv));
		const codeunit_sequence_accessor* accessor = ACCESS(cuq);
		cuq.reserve(10);		// Do nothing
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(33, accessor->as_norm().size);
		EXPECT_EQ(63, accessor->as_norm().capacity);
		EXPECT_EQ(OSTR_UTF8("This is a sentence with 33 words."_cuqv), cuq);
		cuq.reserve(100);
		EXPECT_EQ(33, accessor->as_norm().size);
		EXPECT_EQ(127, accessor->as_norm().capacity);
		EXPECT_EQ(OSTR_UTF8("This is a sentence with 33 words."_cuqv), cuq);
	}
}

TEST(codeunit_sequence, index)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const codeunit_sequence cuq(OSTR_UTF8("abbabbabbabbab"));
		EXPECT_EQ(cuq.index_of(OSTR_UTF8("ab"_cuqv)), 0);
		EXPECT_EQ(cuq.index_of(OSTR_UTF8("ab"_cuqv), { '[', 3, 8, ')' }), 3);
		EXPECT_EQ(cuq.index_of(OSTR_UTF8("ab"_cuqv), { '[', 5, 8, ')' }), 6);
		EXPECT_EQ(cuq.index_of(OSTR_UTF8("ab"_cuqv), { '[', 5, 6, ']' }), index_invalid);
	}
	{
		const codeunit_sequence cuq(OSTR_UTF8("abcab"));
		EXPECT_EQ(cuq.last_index_of(OSTR_UTF8("ab"_cuqv)), 3);
		EXPECT_EQ(cuq.last_index_of(OSTR_UTF8("a"_cuqv), { '[', 0, 2, ')' }), 0);
	}
}

TEST(codeunit_sequence, replace)
{
	SCOPED_DETECT_MEMORY_LEAK
	// same size
	{
		{
			codeunit_sequence cuq(OSTR_UTF8("abbabbabbabbab"));
			EXPECT_EQ(cuq.replace(OSTR_UTF8("ab"_cuqv), OSTR_UTF8("ba"_cuqv)), OSTR_UTF8("babbabbabbabba"_cuqv));
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("abbabbabbabbab"));
			EXPECT_EQ(cuq.replace(OSTR_UTF8("ab"_cuqv), OSTR_UTF8("ba"_cuqv),
				{ '[', 4, 10, ')' }), OSTR_UTF8("abbabbbababbab"_cuqv));
		}
	}
	// smaller size
	{
		{
			codeunit_sequence cuq(OSTR_UTF8("abbabbabbabbab"));
			EXPECT_EQ(cuq.replace(OSTR_UTF8("ab"_cuqv), OSTR_UTF8("a"_cuqv)), OSTR_UTF8("ababababa"_cuqv));
		}
		{
			codeunit_sequence cuq(OSTR_UTF8("abbabbabbabbab"));
			EXPECT_EQ(cuq.replace(OSTR_UTF8("ab"_cuqv), OSTR_UTF8("a"_cuqv), { '[', 4, 10, ')' }), 
				OSTR_UTF8("abbabbababbab"_cuqv));
		}
	}
	// greater size
	{
		{
			{
				codeunit_sequence cuq(OSTR_UTF8("abbabbabbabbab"));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("ab"_cuqv), OSTR_UTF8("aabb"_cuqv)), OSTR_UTF8("aabbbaabbbaabbbaabbbaabb"_cuqv));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("b"_cuqv), OSTR_UTF8("aabb"_cuqv)), OSTR_UTF8("aaaabbaabbaabbaaaabbaabbaabbaaaabbaabbaabbaaaabbaabbaabbaaaabbaabb"_cuqv));
			}
			{
				codeunit_sequence cuq(OSTR_UTF8("abbabbabbabbab"));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("ab"_cuqv), OSTR_UTF8("aabb"_cuqv), { '[', 4, 10, ')' }), OSTR_UTF8("abbabbaabbbabbab"_cuqv));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("b"_cuqv), OSTR_UTF8("aabb"_cuqv), { '[', 6, 100, ')' }), OSTR_UTF8("abbabbaaaabbaabbaabbaaabbaabbaaabb"_cuqv));
			}
		}
		{
			{
				codeunit_sequence cuq(OSTR_UTF8("This is a long string."));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("long"_cuqv), OSTR_UTF8("short"_cuqv)), OSTR_UTF8("This is a short string."_cuqv));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("is"_cuqv), OSTR_UTF8("isn't"_cuqv)), OSTR_UTF8("Thisn't isn't a short string."_cuqv));
			}
			{
				codeunit_sequence cuq(OSTR_UTF8("This is a long string."));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("is"_cuqv), OSTR_UTF8("short"_cuqv), 
					{ '[', 3, 6, ')' }), OSTR_UTF8("This is a long string."_cuqv));
				EXPECT_EQ(cuq.replace(OSTR_UTF8("is"_cuqv), OSTR_UTF8("short"_cuqv), 
					{ '[', 3, 6, ']' }), OSTR_UTF8("This short a long string."_cuqv));
			}
		}
	}
}

TEST(codeunit_sequence, join)
{
	SCOPED_DETECT_MEMORY_LEAK
	{
		const std::vector<codeunit_sequence_view> views = { 
			OSTR_UTF8("This"_cuqv), OSTR_UTF8("is"_cuqv), OSTR_UTF8("a"_cuqv), 
			OSTR_UTF8("very"_cuqv), OSTR_UTF8("very"_cuqv), OSTR_UTF8("long"_cuqv), OSTR_UTF8("text"_cuqv) };
		const codeunit_sequence joined_1 = codeunit_sequence::join(views, OSTR_UTF8("/**/"_cuqv));
		EXPECT_EQ(joined_1, OSTR_UTF8("This/**/is/**/a/**/very/**/very/**/long/**/text"_cuqv));
		const codeunit_sequence joined_2 = codeunit_sequence::join(views, OSTR_UTF8(""_cuqv));
		EXPECT_EQ(joined_2, OSTR_UTF8("Thisisaveryverylongtext"_cuqv));
	}
}
