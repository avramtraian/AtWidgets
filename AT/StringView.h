/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertions.h"
#include "AT/CoreTypes.h"
#include <cstring>

namespace AT
{

//
// A view towards a sequence of immutable UTF-8 encoded characters.
// The viewed string is not null-terminated.
//
class StringView
{
public:
    NODISCARD ALWAYS_INLINE static constexpr StringView
    from_utf8(const char* characters, usize byte_count)
    {
        StringView utf8_view;
        utf8_view.m_characters = characters;
        utf8_view.m_byte_count = byte_count;
        // TODO: Check that the provided string is valid UTF-8?
        return utf8_view;
    }

    NODISCARD ALWAYS_INLINE static StringView from_null_terminated_utf8(const char* characters)
    {
        StringView utf8_view;
        utf8_view.m_characters = characters;
        // TODO: Don't use strlen()!
        utf8_view.m_byte_count = strlen(characters);
        return utf8_view;
    }

public:
    ALWAYS_INLINE constexpr StringView()
        : m_characters(nullptr)
        , m_byte_count(0)
    {
    }

    ALWAYS_INLINE constexpr StringView(const StringView& other)
        : m_characters(other.m_characters)
        , m_byte_count(other.m_byte_count)
    {
    }

    ALWAYS_INLINE constexpr StringView& operator=(const StringView& other)
    {
        m_characters = other.m_characters;
        m_byte_count = other.m_byte_count;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE StringView
    substring(usize offset_in_bytes, usize count_in_bytes) const
    {
        VERIFY(offset_in_bytes + count_in_bytes <= m_byte_count);

        StringView substring;
        substring.m_characters = m_characters + offset_in_bytes;
        substring.m_byte_count = count_in_bytes;
        return substring;
    }

    NODISCARD ALWAYS_INLINE const char& at_offset_in_bytes(usize offset_in_bytes) const
    {
        VERIFY(offset_in_bytes < m_byte_count);
        return m_characters[offset_in_bytes];
    }

public:
    NODISCARD ALWAYS_INLINE const char* characters() const { return m_characters; }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_byte_count == 0); }
    NODISCARD ALWAYS_INLINE ReadonlyBytes bytes() const
    {
        return reinterpret_cast<ReadonlyBytes>(m_characters);
    }

private:
    const char* m_characters;
    usize m_byte_count;
};

} // namespace AT

#if AT_COMPILER_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wuser-defined-literals"
#elif AT_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4455)
#endif // Compilers.

NODISCARD constexpr AT::StringView operator""sv(const char* characters, usize byte_count)
{
    // No encoding conversion is required since ASCII is a subset of UTF-8.
    return AT::StringView::from_utf8(characters, byte_count);
}

#if AT_COMPILER_CLANG
    #pragma clang pop
#elif AT_COMPILER_MSVC
    #pragma warning(pop)
#endif // Compilers.

#if AT_INCLUDE_GLOBALLY
using AT::StringView;
#endif // AT_INCLUDE_GLOBALLY
