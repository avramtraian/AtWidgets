/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Format.h"
#include "AT/MemoryOperations.h"

#define AT_FORMAT_SPECIFIER_BEGIN_TOKEN '{'
#define AT_FORMAT_SPECIFIER_END_TOKEN   '}'

namespace AT
{

void FormatBuilder::push_string(StringView string)
{
    MUST_ASSIGN(auto& offset, m_formatted.try_push_uninitialized(string.byte_count()));
    copy_memory(static_cast<void*>(&offset), string.bytes(), string.byte_count());
}

void FormatBuilder::push_integer(AT::u64 integer, IsNegative is_negative)
{
    if (integer == 0)
    {
        MUST(m_formatted.try_push_back('0'));
        return;
    }

    if (is_negative == IsNegative::Yes)
        MUST(m_formatted.try_push_back('-'));

    u8 digit_count = 0;
    u64 number = integer;
    while (number > 0)
    {
        ++digit_count;
        number /= 10;
    }

    MUST_ASSIGN(char& destination_ref, m_formatted.try_push_uninitialized(digit_count));
    char* destination = &destination_ref;

    number = integer;
    for (u8 digit_index = 1; digit_index <= digit_count; ++digit_index)
    {
        const u8 digit = '0' + number % 10;
        destination[digit_count - digit_index] = static_cast<char>(digit);
        number /= 10;
    }
}

bool FormatBuilder::consume_until_format_specifier()
{
    if (m_format_offset >= m_format.byte_count())
        return false;

    const usize copy_begin_offset = m_format_offset;

    bool has_reached_end = false;
    while (m_format.at_offset_in_bytes(m_format_offset) != AT_FORMAT_SPECIFIER_BEGIN_TOKEN)
    {
        if (++m_format_offset == m_format.byte_count())
        {
            has_reached_end = true;
            break;
        }
    }

    const StringView string_to_copy = m_format.substring(copy_begin_offset, m_format_offset - copy_begin_offset);
    push_string(string_to_copy);

    if (has_reached_end)
        return false;

    ++m_format_offset;
    return true;
}

bool FormatBuilder::consume_format_specifier(Specifier& specifier)
{
    if (m_format_offset == 0 || m_format.at_offset_in_bytes(m_format_offset - 1) != AT_FORMAT_SPECIFIER_BEGIN_TOKEN)
        return false;

    bool has_reached_end = false;
    const usize begin_offset = m_format_offset;

    while (m_format.at_offset_in_bytes(m_format_offset) != AT_FORMAT_SPECIFIER_END_TOKEN)
    {
        if (++m_format_offset == m_format.byte_count())
        {
            has_reached_end = true;
            break;
        }
    }

    if (has_reached_end)
        return false;

    const StringView specifier_string = m_format.substring(begin_offset, m_format_offset - begin_offset);
    ++m_format_offset;
    return process_format_specifier(specifier_string, specifier);
}

bool FormatBuilder::process_format_specifier(AT::StringView specifier_string, AT::FormatBuilder::Specifier& specifier)
{
    if (specifier_string.is_empty())
    {
        specifier = {};
        return true;
    }

    return false;
}

String FormatBuilder::release_as_string()
{
    MUST(m_formatted.try_push_back(0));
    MUST(m_formatted.try_shrink_to_fit());
    String formatted;

    if (m_formatted.count() <= String::InlineCapacity)
    {
        formatted.set_internal_inline_buffer(
            reinterpret_cast<const char*>(m_formatted.bytes()), m_formatted.byte_count()
        );
        m_formatted.clear_and_shrink();
    }
    else
    {
        const usize byte_count = m_formatted.count();
        formatted.set_internal_heap_buffer(m_formatted.leak_elements(), byte_count);
    }

    return formatted;
}

} // namespace AT
