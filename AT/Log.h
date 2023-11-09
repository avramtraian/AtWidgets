/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Format.h"

namespace AT
{

AT_API void dbgln(StringView message);

template<typename... Args>
inline void dbgln(StringView message, Args&&... args)
{
    const String formatted_message = format(message, forward<Args>(args)...);
    dbgln(formatted_message.to_view());
}

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::dbgln;
#endif // AT_INCLUDE_GLOBALLY
