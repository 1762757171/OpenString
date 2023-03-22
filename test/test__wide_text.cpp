
#include "pch.h"

#include "wide_text.h"

using namespace ostr;

TEST(wide_text, construct)
{
	SCOPED_DETECT_MEMORY_LEAK()
	{
		constexpr text_view tv{ "你 好 😙"_txtv };
		const wide_text wt{ tv.raw() };
		u64 index = 0;
		for(const codepoint cp : tv)
		{
#if _WIN64
			const auto* data = reinterpret_cast<const char16_t*>(wt.data());
			const u64 size = unicode::utf16::parse_utf16_length(data[index]);
			EXPECT_EQ(cp.get_codepoint(), unicode::utf16_to_utf32(data + index, size));
			index += size;
#elif __linux__ || __MACH__
			const auto* data = reinterpret_cast<const char32_t*>(wt.data());
			EXPECT_EQ(cp.get_codepoint(), data[index]);
			++index;
#endif
		}
	}
}
