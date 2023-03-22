
#pragma once

#if _WIN64

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <intrin.h>
#include <Windows.h>
#include <debugapi.h>

#define OPEN_STRING_PRINT_SIMPLE_DEBUG_MESSAGE(...)	\
{	\
	[](const char* message_format, auto&&...args)	\
	{	\
		const ostr::codeunit_sequence message = ostr::format(message_format, args...);	\
		OutputDebugStringA(message.c_str());	\
	}(__VA_ARGS__);	\
}	\

#define OPEN_STRING_PRINT_FORMATTED_DEBUG_MESSAGE(...)	\
{	\
	[](const char* debug_format, const char* message_format, auto&&...args)	\
	{	\
		const ostr::codeunit_sequence message = ostr::format(message_format, args...);	\
		OutputDebugStringA(ostr::format(debug_format, message).c_str());	\
	}(__VA_ARGS__);	\
}	\

// Visual Studio will not trigger the breakpoint during single-step debugging without __nop()
#define OPEN_STRING_DEBUG_BREAK() (__nop(), __debugbreak())

#elif defined(__linux__) || defined(__MACH__)

#define OPEN_STRING_PRINT_SIMPLE_DEBUG_MESSAGE(...)
#define OPEN_STRING_PRINT_FORMATTED_DEBUG_MESSAGE(...)
#define OPEN_STRING_DEBUG_BREAK()

#endif
