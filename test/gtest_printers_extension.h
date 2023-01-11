#pragma once
#include <ostream>

#include "common/index_interval.h"
#include "text.h"

NS_EASY_BEGIN

inline std::ostream& operator<<(std::ostream& os, const index_interval& v)
{
	if(v.is_empty())
		return os << "(∅)";
	return os << "[" << v.get_inclusive_min() << ", " << v.get_exclusive_max() << ")";
}

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
	os << v.c_str();
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const text& v)
{
	os << v.view();
	return os;
}

#define DEFINE_PRINT_TO(T) inline void PrintTo(const T& v, std::ostream* os) { *os << v; }

DEFINE_PRINT_TO(text_view)
DEFINE_PRINT_TO(text)

#undef DEFINE_PRINT_TO

NS_EASY_END
