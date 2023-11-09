/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreTypes.h"

namespace AT
{

AT_API void copy_memory(void* destination, const void* source, usize byte_count);
AT_API void set_memory(void* destination, u8 value, usize byte_count);
AT_API void zero_memory(void* destination, usize byte_count);

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::copy_memory;
using AT::set_memory;
using AT::zero_memory;
#endif // AT_INCLUDE_GLOBALLY
