/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/String.h"
#include "AT/MemoryOperations.h"

#include <new>

namespace AT
{

String::String(const String& other)
    : m_byte_count(other.m_byte_count)
{
    if (other.is_stored_inline())
    {
        copy_memory(m_inline_characters, other.m_inline_characters, m_byte_count);
    }
    else
    {
        m_heap_characters = allocate_memory(m_byte_count);
        copy_memory(m_heap_characters, other.m_heap_characters, m_byte_count);
    }
}

String::String(String&& other) noexcept
    : m_byte_count(other.m_byte_count)
    , m_heap_characters(other.m_heap_characters)
{
    other.m_heap_characters = nullptr;
    other.m_byte_count = 1;
}

String::String(StringView view)
    : m_byte_count(view.byte_count() + 1)
{
    if (is_stored_inline())
    {
        copy_memory(m_inline_characters, view.characters(), view.byte_count());
        m_inline_characters[m_byte_count - 1] = 0;
    }
    else
    {
        m_heap_characters = allocate_memory(m_byte_count);
        copy_memory(m_heap_characters, view.characters(), view.byte_count());
        m_heap_characters[m_byte_count - 1] = 0;
    }
}

String& String::operator=(const String& other)
{
    if (this == &other)
        return *this;

    char* destination_buffer;
    const char* source_buffer;

    if (is_stored_inline())
    {
        if (other.is_stored_inline())
        {
            destination_buffer = m_inline_characters;
            source_buffer = other.m_inline_characters;
        }
        else
        {
            m_heap_characters = allocate_memory(other.m_byte_count);
            destination_buffer = m_heap_characters;
            source_buffer = other.m_heap_characters;
        }
    }
    else
    {
        if (other.is_stored_inline())
        {
            release_memory(m_heap_characters, m_byte_count);
            destination_buffer = m_inline_characters;
            source_buffer = other.m_inline_characters;
        }
        else
        {
            if (m_byte_count != other.m_byte_count)
            {
                release_memory(m_heap_characters, m_byte_count);
                m_heap_characters = allocate_memory(other.m_byte_count);
            }

            destination_buffer = m_heap_characters;
            source_buffer = other.m_heap_characters;
        }
    }

    m_byte_count = other.m_byte_count;
    copy_memory(destination_buffer, source_buffer, m_byte_count);

    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (this == &other)
        return *this;

    if (is_stored_on_heap())
        release_memory(m_heap_characters, m_byte_count);

    m_byte_count = other.m_byte_count;
    m_heap_characters = other.m_heap_characters;

    other.m_heap_characters = nullptr;
    other.m_byte_count = 1;

    return *this;
}

String& String::operator=(StringView view)
{
    char* destination_buffer;
    const usize source_byte_count = view.byte_count() + 1;

    if (is_stored_inline())
    {
        if (source_byte_count <= InlineCapacity)
        {
            destination_buffer = m_inline_characters;
        }
        else
        {
            m_heap_characters = allocate_memory(source_byte_count);
            destination_buffer = m_heap_characters;
        }
    }
    else
    {
        if (source_byte_count <= InlineCapacity)
        {
            release_memory(m_heap_characters, m_byte_count);
            destination_buffer = m_inline_characters;
        }
        else
        {
            if (m_byte_count != source_byte_count)
            {
                release_memory(m_heap_characters, m_byte_count);
                m_heap_characters = allocate_memory(source_byte_count);
            }

            destination_buffer = m_heap_characters;
        }
    }

    m_byte_count = source_byte_count;
    copy_memory(destination_buffer, view.characters(), view.byte_count());
    destination_buffer[m_byte_count - 1] = 0;

    return *this;
}

String::~String()
{
    if (is_stored_on_heap())
        release_memory(m_heap_characters, m_byte_count);
}

void String::set_internal_heap_buffer(char* heap_characters, usize byte_count)
{
    VERIFY(byte_count > InlineCapacity);

    if (is_stored_on_heap())
        release_memory(m_heap_characters, m_byte_count);

    m_heap_characters = heap_characters;
    m_byte_count = byte_count;
}

void String::set_internal_inline_buffer(const char* inline_characters, usize byte_count)
{
    VERIFY(byte_count <= InlineCapacity);
    if (is_stored_on_heap())
        release_memory(m_heap_characters, m_byte_count);

    copy_memory(m_inline_characters, inline_characters, byte_count);
    m_byte_count = byte_count;
}

char* String::allocate_memory(usize byte_count)
{
    // TODO: Custom memory allocators.
    void* memory = ::operator new(byte_count);
    return reinterpret_cast<char*>(memory);
}

void String::release_memory(char* characters, usize byte_count)
{
    // TODO: Custom memory allocators.
    ::operator delete(characters, byte_count);
}

} // namespace AT
