/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/MemoryOperations.h"

namespace AT
{

void copy_memory(void* destination, const void* source, usize byte_count)
{
    u8* dst = reinterpret_cast<u8*>(destination);
    const u8* src = reinterpret_cast<const u8*>(source);
    while (byte_count--)
        *dst++ = *src++;
}

void set_memory(void* destination, u8 value, usize byte_count)
{
    u8* dst = reinterpret_cast<u8*>(destination);
    while (byte_count--)
        *dst++ = value;
}

void zero_memory(void* destination, usize byte_count)
{
    u8* dst = reinterpret_cast<u8*>(destination);
    while (byte_count--)
        *dst++ = 0;
}

} // namespace AT
