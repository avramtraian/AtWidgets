/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertions.h"
#include "AT/CoreTypes.h"

namespace AT
{

template<typename T>
class Span
{
public:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = T*;
    using ReverseConstIterator = const T*;

public:
    ALWAYS_INLINE Span()
        : m_elements(nullptr)
        , m_count(0)
    {
    }

    ALWAYS_INLINE Span(const Span& other)
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {
    }

    ALWAYS_INLINE Span(T* elements, usize count)
        : m_elements(elements)
        , m_count(count)
    {
    }

    ALWAYS_INLINE Span& operator=(const Span& other)
    {
        m_elements = other.m_elements;
        m_count = other.m_count;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE T& at(usize index)
    {
        VERIFY(index < m_count);
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE const T& at(usize index) const
    {
        VERIFY(index < m_count);
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE T& operator[](usize index) { return at(index); }
    NODISCARD ALWAYS_INLINE const T& operator[](usize index) const { return at(index); }

public:
    NODISCARD ALWAYS_INLINE T* elements() { return m_elements; }
    NODISCARD ALWAYS_INLINE const T* elements() const { return m_elements; }

    NODISCARD ALWAYS_INLINE usize count() const { return m_count; }
    NODISCARD ALWAYS_INLINE usize is_empty() const { return m_count == 0; }
    NODISCARD ALWAYS_INLINE usize element_size() const { return sizeof(T); }

    NODISCARD ALWAYS_INLINE ReadWriteBytes bytes() { return reinterpret_cast<ReadWriteBytes>(m_elements); }
    NODISCARD ALWAYS_INLINE ReadonlyBytes bytes() const { return reinterpret_cast<ReadonlyBytes>(m_elements); }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_count * sizeof(T); }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return Iterator(m_elements); }
    NODISCARD ALWAYS_INLINE Iterator end() { return Iterator(m_elements + m_count); }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return ConstIterator(m_elements); }
    NODISCARD ALWAYS_INLINE ConstIterator end() const { return ConstIterator(m_elements + m_count); }

    NODISCARD ALWAYS_INLINE ReverseIterator rbegin() { return ReverseIterator(m_elements + m_count - 1); }
    NODISCARD ALWAYS_INLINE ReverseIterator rend() { return ReverseIterator(m_elements - 1); }

    NODISCARD ALWAYS_INLINE ReverseConstIterator rbegin() const
    {
        return ReverseConstIterator(m_elements + m_count - 1);
    }
    NODISCARD ALWAYS_INLINE ReverseConstIterator rend() const { return ReverseConstIterator(m_elements - 1); }

private:
    T* m_elements;
    usize m_count;
};

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::Span;
#endif // AT_INCLUDE_GLOBALLY
