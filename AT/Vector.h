/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertions.h"
#include "AT/CoreTypes.h"
#include "AT/Span.h"

#include <new>

namespace AT
{

//
// Dynamic collection of elements that are stored contiguously in memory.
// The type of elements stored in this container must provide the ability
// to be moved in memory, as this operation is performed every time the
// vector grows, shrinks or the elements are shifted.
//
template<typename T>
class Vector
{
public:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = T*;
    using ReverseConstIterator = const T*;

public:
    ALWAYS_INLINE Vector()
        : m_elements(nullptr)
        , m_capacity(0)
        , m_count(0)
    {
    }

    ALWAYS_INLINE Vector(const Vector& other)
        : m_count(other.m_count)
    {
        m_capacity = m_count;
        m_elements = allocate_memory(m_capacity);
        copy_elements(m_elements, other.m_elements, m_count);
    }

    ALWAYS_INLINE Vector(Vector&& other) noexcept
        : m_elements(other.m_elements)
        , m_capacity(other.m_capacity)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;
    }

    ALWAYS_INLINE Vector& operator=(const Vector& other)
    {
        if (this == &other)
            return *this;

        clear();

        if (other.m_count > m_capacity)
        {
            release_memory(m_elements, m_capacity);
            m_capacity = calculate_next_capacity(m_capacity, other.m_count);
            m_elements = allocate_memory(m_capacity);
        }

        m_count = other.m_count;
        copy_elements(m_elements, other.m_elements, m_count);

        return *this;
    }

    ALWAYS_INLINE Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other)
            return *this;

        clear();
        release_memory(m_elements, m_capacity);

        m_elements = other.m_elements;
        m_capacity = other.m_capacity;
        m_count = other.m_count;

        other.m_elements = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;

        return *this;
    }

    ALWAYS_INLINE ~Vector()
    {
        clear();
        release_memory(m_elements, m_capacity);
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

    NODISCARD ALWAYS_INLINE T& first() { return at(0); }
    NODISCARD ALWAYS_INLINE const T& first() const { return at(0); }

    NODISCARD ALWAYS_INLINE T& last()
    {
        VERIFY(m_count > 0);
        return m_elements[m_count - 1];
    }

    NODISCARD ALWAYS_INLINE const T& last() const
    {
        VERIFY(m_count > 0);
        return m_elements[m_count - 1];
    }

public:
    ALWAYS_INLINE T& push_uninitialized(usize count = 1)
    {
        reallocate_if_required(m_count + count);
        m_count += count;
        return m_elements[m_count - count];
    }

    template<typename... Args>
    ALWAYS_INLINE T& emplace_back(Args&&... args)
    {
        T& slot = push_uninitialized(1);
        new (&slot) T(forward<Args>(args)...);
        return slot;
    }

    ALWAYS_INLINE T& push_back(const T& value) { return emplace_back(value); }
    ALWAYS_INLINE T& push_back(T&& value) { return emplace_back(move(value)); }

public:
    ALWAYS_INLINE void clear()
    {
        for (usize index = 0; index < m_count; ++index)
            m_elements[index].~T();
        m_count = 0;
    }

    ALWAYS_INLINE void clear_and_shrink()
    {
        clear();
        release_memory(m_elements, m_capacity);
        m_elements = nullptr;
        m_capacity = 0;
    }

    ALWAYS_INLINE void shrink_to_fit()
    {
        if (m_count < m_capacity)
            reallocate_to_fixed(m_count);
    }

    ALWAYS_INLINE void pop_back()
    {
        VERIFY(m_count > 0);
        m_elements[--m_count].~T();
    }

    ALWAYS_INLINE void pop_back(usize count)
    {
        VERIFY(m_count >= count);
        for (usize index = m_count - count; index < m_count; ++index)
            m_elements[index].~T();
        m_count -= count;
    }

    ALWAYS_INLINE T take_last()
    {
        VERIFY(m_count > 0);
        T last = move(m_elements[m_count - 1]);
        m_elements[--m_count].~T();
        return last;
    }

public:
    NODISCARD ALWAYS_INLINE T* elements() { return m_elements; }
    NODISCARD ALWAYS_INLINE const T* elements() const { return m_elements; }

    NODISCARD ALWAYS_INLINE usize count() const { return m_count; }
    NODISCARD ALWAYS_INLINE usize capacity() const { return m_capacity; }
    NODISCARD ALWAYS_INLINE usize available() { return m_capacity - m_count; }
    NODISCARD ALWAYS_INLINE usize element_size() const { return sizeof(T); }

    NODISCARD ALWAYS_INLINE ReadWriteBytes bytes()
    {
        return reinterpret_cast<ReadWriteBytes>(m_elements);
    }
    NODISCARD ALWAYS_INLINE ReadonlyBytes bytes() const
    {
        return reinterpret_cast<ReadonlyBytes>(m_elements);
    }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_count * sizeof(T); }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return Iterator(m_elements); }
    NODISCARD ALWAYS_INLINE Iterator end() { return Iterator(m_elements + m_count); }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return ConstIterator(m_elements); }
    NODISCARD ALWAYS_INLINE ConstIterator end() const
    {
        return ConstIterator(m_elements + m_count);
    }

    NODISCARD ALWAYS_INLINE ReverseIterator rbegin()
    {
        return ReverseIterator(m_elements + m_count - 1);
    }
    NODISCARD ALWAYS_INLINE ReverseIterator rend() { return ReverseIterator(m_elements - 1); }

    NODISCARD ALWAYS_INLINE ReverseConstIterator rbegin() const
    {
        return ReverseConstIterator(m_elements + m_count - 1);
    }
    NODISCARD ALWAYS_INLINE ReverseConstIterator rend() const
    {
        return ReverseConstIterator(m_elements - 1);
    }

public:
    // IMPORTANT: This function causes a memory leak if not used correctly. Only intended for
    // low-level operations.
    AT_DANGEROUS NODISCARD ALWAYS_INLINE T* leak_elements()
    {
        T* elements = m_elements;
        m_elements = nullptr;
        m_capacity = 0;
        m_count = 0;
        return elements;
    }

private:
    NODISCARD ALWAYS_INLINE static T* allocate_memory(usize capacity)
    {
        // TODO: Custom memory allocators.
        void* memory_block = ::operator new(capacity * sizeof(T));
        return reinterpret_cast<T*>(memory_block);
    }

    ALWAYS_INLINE static void release_memory(T* elements, usize capacity)
    {
        // TODO: Custom memory allocators.
        ::operator delete(elements, capacity * sizeof(T));
    }

    ALWAYS_INLINE static void copy_elements(T* destination, const T* source, usize count)
    {
        for (usize index = 0; index < count; ++index)
            new (destination + index) T(source[index]);
    }

    ALWAYS_INLINE static void move_elements(T* destination, T* source, usize count)
    {
        for (usize index = 0; index < count; ++index)
        {
            new (destination + index) T(move(source[index]));
            source[index].~T();
        }
    }

    // Without external constraints, the capacity of the vector follows a geometric series, with
    // the ratio of 1.5. If the element in the geometric series is not sufficient,
    // 'required_capacity' will be returned instead.
    NODISCARD ALWAYS_INLINE static constexpr usize
    calculate_next_capacity(usize current_capacity, usize required_capacity)
    {
        const usize next_geometric_capacity = current_capacity + current_capacity / 2;
        if (required_capacity > next_geometric_capacity)
            return required_capacity;
        return next_geometric_capacity;
    }

private:
    ALWAYS_INLINE void reallocate_to_fixed(usize new_capacity)
    {
        VERIFY(new_capacity >= m_count);

        T* new_elements = allocate_memory(new_capacity);
        move_elements(new_elements, m_elements, m_count);
        release_memory(m_elements, m_capacity);

        m_elements = new_elements;
        m_capacity = new_capacity;
    }

    ALWAYS_INLINE void reallocate(usize required_capacity)
    {
        const usize new_capacity = calculate_next_capacity(m_capacity, required_capacity);
        reallocate_to_fixed(new_capacity);
    }

    ALWAYS_INLINE void reallocate_if_required(usize required_capacity)
    {
        if (required_capacity > m_capacity)
            reallocate(required_capacity);
    }

private:
    T* m_elements;
    usize m_capacity;
    usize m_count;
};

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::Vector;
#endif // AT_INCLUDE_GLOBALLY