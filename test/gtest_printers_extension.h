
#pragma once

#include <ostream>
#include "unicode.h"
#include "text.h"
#include "format.h"

#define DEFINE_PRINT_TO(T) inline void PrintTo(const T& v, std::ostream* os) { *os << v; }

namespace ostr
{
	inline std::ostream& operator<<(std::ostream& os, const codeunit_sequence_view& v)
	{
		for(const auto c : v)
			os << c;
		return os;
	}
	
	inline std::ostream& operator<<(std::ostream& os, const codeunit_sequence& v)
	{
		return os << v.view();
	}

	inline std::ostream& operator<<(std::ostream& os, const codepoint& v)
	{
		const codeunit_sequence s = codeunit_sequence::join(v, ", "_cuqv);
		os << "[ " << s << " ] = " << v.get_codepoint();
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const text_view& v)
	{
		os << v.raw();
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const text& v)
	{
		os << v.view();
		return os;
	}

DEFINE_PRINT_TO(codeunit_sequence_view)
DEFINE_PRINT_TO(codeunit_sequence)
DEFINE_PRINT_TO(text_view)
DEFINE_PRINT_TO(text)
}
