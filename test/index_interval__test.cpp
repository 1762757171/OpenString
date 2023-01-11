#include "pch.h"
#include "common/index_interval.h"

using namespace easy;

TEST(index_interval, intersect)
{
	{
		constexpr index_interval lhs { '~', 6, ')' };
		constexpr index_interval rhs { '[', 4, '~' };
		constexpr index_interval answer = lhs.intersect(rhs);
		constexpr index_interval expect { '[', 4, 6, ')' };
		EXPECT_EQ(answer, expect);
	}
	{
		constexpr index_interval lhs { '[', 2, 8, ')' };
		constexpr index_interval rhs { '(', 5, 7, ']' };
		constexpr index_interval answer = lhs.intersect(rhs);
		constexpr index_interval expect { '(', 5, 7, ']' };
		EXPECT_EQ(answer, expect);
	}
}

