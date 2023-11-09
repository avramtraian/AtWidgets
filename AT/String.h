/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreTypes.h"
#include "AT/StringView.h"

namespace AT
{

//
// Container that stored a UTF-8 encoded, null-terminated string.
// Depending on the size and the platform, the string might be stored inline. In
// this case, no memory allocation is performed. Otherwise, the memory is dynamically
// allocated from the heap.
//
class String
{
public:
    // The number of bytes, including the null-termination byte, that can be stored inline.
    static constexpr usize InlineCapacity = sizeof(char*);

public:
    ALWAYS_INLINE String()
        : m_byte_count(1)
    {
        m_heap_characters = nullptr;
    }

    AT_API String(const String& other);
    AT_API String(String&& other) noexcept;
    AT_API String(StringView view);

    AT_API String& operator=(const String& other);
    AT_API String& operator=(String&& other) noexcept;
    AT_API String& operator=(StringView view);

    AT_API ~String();

public:
    NODISCARD ALWAYS_INLINE StringView to_view() const
    {
        return StringView::from_utf8(characters(), m_byte_count - 1);
    }

    NODISCARD ALWAYS_INLINE const char* characters() const
    {
        return reinterpret_cast<const char*>(bytes());
    }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }

    NODISCARD ALWAYS_INLINE ReadonlyBytes bytes() const
    {
        return is_stored_on_heap() ? reinterpret_cast<ReadonlyBytes>(m_heap_characters)
                                   : reinterpret_cast<ReadonlyBytes>(m_inline_characters);
    }

    NODISCARD ALWAYS_INLINE ReadWriteBytes bytes()
    {
        return is_stored_on_heap() ? reinterpret_cast<ReadWriteBytes>(m_heap_characters)
                                   : reinterpret_cast<ReadWriteBytes>(m_inline_characters);
    }

    NODISCARD ALWAYS_INLINE bool is_stored_inline() const
    {
        return (m_byte_count <= InlineCapacity);
    }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const
    {
        return (m_byte_count > InlineCapacity);
    }

public:
    // IMPORTANT: Sets the string characters pointer to the specified value. This function is
    // dangerous to use and should only be limited to low-level operations.
    // No null-termination character will be inserted.
    // After this call, the lifetime of the provided memory is managed ENTIRELY by the string.
    AT_DANGEROUS AT_API void set_internal_heap_buffer(char* heap_characters, usize byte_count);

    // IMPORTANT: Copies the characters from the provided string to the inline buffer.
    // No null-termination character will be inserted.
    // The lifetime of the passed buffer will not be altered in any way.
    AT_DANGEROUS AT_API void
    set_internal_inline_buffer(const char* inline_characters, usize byte_count);

private:
    NODISCARD static char* allocate_memory(usize byte_count);
    static void release_memory(char* characters, usize byte_count);

private:
    union
    {
        char* m_heap_characters;
        char m_inline_characters[InlineCapacity];
    };

    usize m_byte_count;
};

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::String;
#endif // AT_INCLUDE_GLOBALLY
