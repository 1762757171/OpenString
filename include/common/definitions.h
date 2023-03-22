// OpenString - definitions
//
// Copyright (c) 2022 - present, [Hoshizora Ming]
// All rights reserved.
#pragma once

#ifndef OPEN_STRING_NS
#define OPEN_STRING_NS easy
#endif

#ifndef OPEN_STRING_NS_BEGIN
#define OPEN_STRING_NS_BEGIN	namespace OPEN_STRING_NS {
#endif

#ifndef OPEN_STRING_NS_END
#define OPEN_STRING_NS_END		}
#endif

#ifndef OPEN_STRING_API
#define OPEN_STRING_API
#endif

#ifndef OPEN_STRING_STRINGIFY
#define OPEN_STRING_STRINGIFY(x) #x
#endif
#ifndef OPEN_STRING_STRINGIFY_EXPANDED
#define OPEN_STRING_STRINGIFY_EXPANDED(x) OPEN_STRING_STRINGIFY(x)
#endif

#include "platforms.h"

#ifndef OPEN_STRING_UNLIKELY
#define OPEN_STRING_UNLIKELY(expression)    (!!(expression))
#endif

#ifndef OPEN_STRING_PRINT_DEBUG_MESSAGE
#define OPEN_STRING_PRINT_DEBUG_MESSAGE(...)
#endif

#ifndef OPEN_STRING_DEBUG_BREAK
#define OPEN_STRING_DEBUG_BREAK()
#endif

#ifndef OPEN_STRING_CHECK
#define OPEN_STRING_CHECK(expression, ...)  \
    { \
        if(OPEN_STRING_UNLIKELY(!(expression))) \
        { \
            OPEN_STRING_PRINT_DEBUG_MESSAGE("\nCheck failed: {}\n\t[" __FILE__ ":" OPEN_STRING_STRINGIFY_EXPANDED(__LINE__) "] " OPEN_STRING_STRINGIFY(expression) "\n", __VA_ARGS__ ); \
            OPEN_STRING_DEBUG_BREAK(); \
        } \
    }
#endif
