/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreTypes.h"

#define VERIFY(expression)                                                 \
    if (!(expression))                                                     \
    {                                                                      \
        verification_failed(#expression, __FILE__, AT_FUNCTION, __LINE__); \
        AT_DEBUGBREAK;                                                     \
    }

#define INVALID_CODEPATH                                                               \
    verification_failed("Invalid codepath reached!", __FILE__, AT_FUNCTION, __LINE__); \
    AT_DEBUGBREAK;

namespace AT
{

AT_API void
verification_failed(const char* expression, const char* file, const char* function, u32 line);

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::verification_failed;
#endif // AT_INCLUDE_GLOBALLY
