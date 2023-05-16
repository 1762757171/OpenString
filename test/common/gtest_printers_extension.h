#pragma once
#include <ostream>
#include "common/index_interval.h"
#include "text.h"
#include "format.h"

OPEN_STRING_NS_BEGIN

inline std::ostream& operator<<(std::ostream& os, const codeunit_sequence_view& v)
{
	for(const auto c : v)
		os << (const char)c;
	return os;
}
	
inline std::ostream& operator<<(std::ostream& os, const codeunit_sequence& v)
{
	return os << v.view();
}

inline std::ostream& operator<<(std::ostream& os, const index_interval& v)
{
	return os << format(OSTR_UTF8("{}"_cuqv), v);
}

inline std::ostream& operator<<(std::ostream& os, const codepoint& v)
{
	const codeunit_sequence s = codeunit_sequence::join(v, OSTR_UTF8(", "_cuqv));
	os << "[ " << s << " ] = " << (const char)v.get_codepoint();
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const text_view& v)
{
	os << v.c_str();
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const text& v)
{
	os << v.view();
	return os;
}

#define DEFINE_PRINT_TO(T) inline void PrintTo(const T& v, std::ostream* os) { *os << v; }

DEFINE_PRINT_TO(codeunit_sequence_view)
DEFINE_PRINT_TO(codeunit_sequence)
DEFINE_PRINT_TO(index_interval)
DEFINE_PRINT_TO(text_view)
DEFINE_PRINT_TO(text)

#undef DEFINE_PRINT_TO

OPEN_STRING_NS_END
