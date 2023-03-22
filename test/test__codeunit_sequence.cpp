
// ReSharper disable StringLiteralTypo
#include "pch.h"

using namespace ostr;

// this is a struct that has same memory layout as struct codeunit_sequence
// convert the pointer of struct codeunit_sequence to this struct and access the private members.
struct codeunit_sequence_accessor
{
	static constexpr u64 SSO_SIZE_MAX = 14;

	struct sso
	{
		u8 alloc : 1;
		u8 size : 7;
		std::array<char, SSO_SIZE_MAX + 1> data;
	};

	struct norm
	{
		u32 alloc : 1;
		u32 size : 15;
		u32 capacity;
		char* data;
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

	std::array<char, 16> store;
};

#define ACCESS(cuq) ((codeunit_sequence_accessor*)(&(cuq)))

TEST(codeunit_sequence, construct)
{
	SCOPED_DETECT_MEMORY_LEAK()
	
	const codeunit_sequence cuq_default_ctor;
	const codeunit_sequence cuq_ctor_supplement("𪚥😁");
	const codeunit_sequence cuq_ctor_long("👌𪚥1😁 你好");

	EXPECT_EQ(0, cuq_default_ctor.size());
	EXPECT_EQ(8, cuq_ctor_supplement.size());
	EXPECT_EQ(20, cuq_ctor_long.size());
}

TEST(codeunit_sequence, concatenate)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		const codeunit_sequence cuq_ai("😘");

		const codeunit_sequence combine = codeunit_sequence::build("我", cuq_ai, cuq_ai, codeunit_sequence("あなた"), "!"_cuqv);
		const codeunit_sequence correct("我😘😘あなた!");

		EXPECT_EQ(combine, correct);
	}
}

TEST(codeunit_sequence, split)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		const codeunit_sequence s("This        is   a long string"_cuqv);
		std::vector<codeunit_sequence_view> result;
		s.split(" "_cuqv, result);
		const std::vector<codeunit_sequence_view> answer { "This"_cuqv, "is"_cuqv, "a"_cuqv, "long"_cuqv, "string"_cuqv };
		EXPECT_EQ(result, answer);
	}
}

TEST(codeunit_sequence, trim)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{ 
		{
			codeunit_sequence cuq_src("   123 1234    ");
			cuq_src.self_trim_start();
			EXPECT_EQ(cuq_src, "123 1234    ");
		}
		{
			codeunit_sequence cuq_src("   123 1234   ");
			cuq_src.self_trim_end();
			EXPECT_EQ(cuq_src, "   123 1234");
		}
		{
			codeunit_sequence cuq_src("   123 1234   ");
			cuq_src.self_trim();
			EXPECT_EQ(cuq_src, "123 1234");
		}
	}

	{
		{
			const codeunit_sequence cuq_origin("   你 好 😙  \t  ");
			codeunit_sequence cuq = cuq_origin;
			EXPECT_EQ(cuq, "   你 好 😙  \t  ");
			cuq.self_trim_start();
			EXPECT_EQ(cuq, "你 好 😙  \t  ");
		}
		{
			codeunit_sequence cuq("   你 好 😙  \t  ");
			EXPECT_EQ(cuq.self_trim_end(), "   你 好 😙");
		}
		{
			codeunit_sequence cuq("   你 好 😙  \t  "_cuqv);
			EXPECT_EQ(cuq.self_trim(), "你 好 😙");
		}
	}

	{
		codeunit_sequence cuq("你 好 😙");
		EXPECT_EQ(cuq.self_trim_start(), "你 好 😙");
		EXPECT_EQ(cuq.self_trim_end(), "你 好 😙");
		EXPECT_EQ(cuq.self_trim(), "你 好 😙");
	}
	{
		{
			codeunit_sequence cuq("       ");
			EXPECT_EQ(cuq.self_trim_start(), "");
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq("       ");
			EXPECT_EQ(cuq.self_trim_end(), "");
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq("       ");
			EXPECT_EQ(cuq.self_trim(), "");
			EXPECT_TRUE(cuq.is_empty());
		}
	}
	{
		{
			codeunit_sequence cuq;
			EXPECT_EQ(cuq.self_trim_start(), "");
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq("       ");
			EXPECT_EQ(cuq.self_trim_end(), "");
			EXPECT_TRUE(cuq.is_empty());
		}
		{
			codeunit_sequence cuq("       ");
			EXPECT_EQ(cuq.self_trim(), "");
			EXPECT_TRUE(cuq.is_empty());
		}
	}
	{
		{
			codeunit_sequence cuq("你 好 😙"_cuqv);
			EXPECT_EQ(cuq.self_trim_start("你 😙"_cuqv), "好 😙"_cuqv);
		}
		{
			codeunit_sequence cuq("你 好 😙"_cuqv);
			EXPECT_EQ(cuq.self_trim_end("你 😙"_cuqv), "你 \xE5\xA5"_cuqv);
		}
		{
			codeunit_sequence cuq("你 好 😙"_cuqv);
			EXPECT_EQ(cuq.self_trim("你 😙"_cuqv), "\xE5\xA5"_cuqv);
		}
	}
}

TEST(codeunit_sequence, empty)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		codeunit_sequence cuq("12345");
		cuq.empty(50);
		const codeunit_sequence_accessor* accessor = ACCESS(cuq);
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(0, accessor->as_norm().size);
		EXPECT_EQ(63, accessor->as_norm().capacity);
	}
	{
		codeunit_sequence cuq("This is a sentence with 33 words."_cuqv);
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
	SCOPED_DETECT_MEMORY_LEAK()
	{
		codeunit_sequence cuq("12345");
		const codeunit_sequence_accessor* accessor = ACCESS(cuq);
		cuq.reserve(10);		// Do nothing
		EXPECT_TRUE(accessor->is_short());
		EXPECT_EQ(5, accessor->as_sso().size);
		EXPECT_EQ("12345"_cuqv, cuq);
		cuq.reserve(50);
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(5, accessor->as_norm().size);
		EXPECT_EQ(63, accessor->as_norm().capacity);
	}
	{
		codeunit_sequence cuq("This is a sentence with 33 words."_cuqv);
		const codeunit_sequence_accessor* accessor = ACCESS(cuq);
		cuq.reserve(10);		// Do nothing
		EXPECT_TRUE(!accessor->is_short());
		EXPECT_EQ(33, accessor->as_norm().size);
		EXPECT_EQ(63, accessor->as_norm().capacity);
		EXPECT_EQ("This is a sentence with 33 words."_cuqv, cuq);
		cuq.reserve(100);
		EXPECT_EQ(33, accessor->as_norm().size);
		EXPECT_EQ(127, accessor->as_norm().capacity);
		EXPECT_EQ("This is a sentence with 33 words."_cuqv, cuq);
	}
}

TEST(codeunit_sequence, index)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		const codeunit_sequence cuq("abbabbabbabbab");
		EXPECT_EQ(cuq.index_of("ab"_cuqv), 0);
		EXPECT_EQ(cuq.index_of("ab"_cuqv, 3, 5), 3);
		EXPECT_EQ(cuq.index_of("ab"_cuqv, 5, 3), 6);
		EXPECT_EQ(cuq.index_of("ab"_cuqv, 5, 2), global_constant::INDEX_INVALID);
	}
	{
		const codeunit_sequence cuq("abcab");
		EXPECT_EQ(cuq.last_index_of("ab"_cuqv), 3);
		EXPECT_EQ(cuq.last_index_of("a"_cuqv, 0, 2), 0);
	}
}

TEST(codeunit_sequence, replace)
{
	SCOPED_DETECT_MEMORY_LEAK()
	// same size
	{
		{
			codeunit_sequence cuq("abbabbabbabbab");
			EXPECT_EQ(cuq.replace("ba"_cuqv, "ab"_cuqv), "babbabbabbabba"_cuqv);
		}
		{
			codeunit_sequence cuq("abbabbabbabbab");
			EXPECT_EQ(cuq.replace("ba"_cuqv, "ab"_cuqv, 4, 6), "abbabbbababbab"_cuqv);
		}
	}
	// smaller size
	{
		{
			codeunit_sequence cuq("abbabbabbabbab");
			EXPECT_EQ(cuq.replace("a"_cuqv, "ab"_cuqv), "ababababa"_cuqv);
		}
		{
			codeunit_sequence cuq("abbabbabbabbab");
			EXPECT_EQ(cuq.replace("a"_cuqv, "ab"_cuqv, 4, 6), "abbabbababbab"_cuqv);
		}
	}
	// greater size
	{
		{
			{
				codeunit_sequence cuq("abbabbabbabbab");
				EXPECT_EQ(cuq.replace("aabb"_cuqv, "ab"_cuqv), "aabbbaabbbaabbbaabbbaabb"_cuqv);
				EXPECT_EQ(cuq.replace("aabb"_cuqv, "b"_cuqv), "aaaabbaabbaabbaaaabbaabbaabbaaaabbaabbaabbaaaabbaabbaabbaaaabbaabb"_cuqv);
			}
			{
				codeunit_sequence cuq("abbabbabbabbab");
				EXPECT_EQ(cuq.replace("aabb"_cuqv, "ab"_cuqv, 4, 6), "abbabbaabbbabbab"_cuqv);
				EXPECT_EQ(cuq.replace("aabb"_cuqv, "b"_cuqv, 6, 100), "abbabbaaaabbaabbaabbaaabbaabbaaabb"_cuqv);
			}
		}
		{
			{
				codeunit_sequence cuq("This is a long string.");
				EXPECT_EQ(cuq.replace("short"_cuqv, "long"_cuqv), "This is a short string."_cuqv);
				EXPECT_EQ(cuq.replace("isn't"_cuqv, "is"_cuqv), "Thisn't isn't a short string."_cuqv);
			}
			{
				codeunit_sequence cuq("This is a long string.");
				EXPECT_EQ(cuq.replace("short"_cuqv, "is"_cuqv, 3, 3), "This is a long string."_cuqv);
				EXPECT_EQ(cuq.replace("short"_cuqv, "is"_cuqv, 3, 4), "This short a long string."_cuqv);
			}
		}
	}
}

TEST(codeunit_sequence, join)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		const sequence<codeunit_sequence_view> views = { "This"_cuqv, "is"_cuqv, "a"_cuqv, "very"_cuqv, "very"_cuqv, "long"_cuqv, "text"_cuqv };
		const codeunit_sequence joined_1 = codeunit_sequence::join(views, "/**/"_cuqv);
		EXPECT_EQ(joined_1, "This/**/is/**/a/**/very/**/very/**/long/**/text"_cuqv);
		const codeunit_sequence joined_2 = codeunit_sequence::join(views, ""_cuqv);
		EXPECT_EQ(joined_2, "Thisisaveryverylongtext"_cuqv);
	}
}
