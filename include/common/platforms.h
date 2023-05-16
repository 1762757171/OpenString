#pragma once

#if _WIN64

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <intrin.h>
#include <Windows.h>
#include <debugapi.h>

#define OPEN_STRING_PRINT_DEBUG_MESSAGE(...)	\
{	\
	[](const char* debug_format, const char* message_format, auto&&...args)	\
	{	\
		const easy::codeunit_sequence message = easy::format(message_format, args...);	\
		OutputDebugStringA(easy::format(debug_format, message).c_str());	\
	}(__VA_ARGS__);	\
}	\

// Visual Studio will not trigger the breakpoint during single-step debugging without __nop()
#define OPEN_STRING_DEBUG_BREAK() (__nop(), __debugbreak())

#endif
