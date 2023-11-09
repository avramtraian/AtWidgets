/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/String.h"
#include "AT/Vector.h"

namespace AT
{

enum class IsNegative : u8
{
    No = 0,
    Yes = 1,
};

class FormatBuilder
{
public:
    struct Specifier
    {
    };

public:
    FormatBuilder(StringView format)
        : m_format(format)
    {
    }

    AT_API void push_string(StringView string);
    AT_API void push_integer(u64 integer, IsNegative is_negative);

    // Returns true if a format specifier was found.
    AT_API bool consume_until_format_specifier();
    // Returns true if the format specifier is valid.
    AT_API bool consume_format_specifier(Specifier& specifier);

    AT_API String release_as_string();

private:
    static bool process_format_specifier(StringView specifier_string, Specifier& specifier);

private:
    StringView m_format;
    usize m_format_offset = 0;

    Vector<char> m_formatted;
};

template<typename T>
struct Formatter
{
    static void format(FormatBuilder&, const FormatBuilder::Specifier&, const T&) {}
};

template<>
struct Formatter<u8>
{
    static void format(FormatBuilder& builder, const FormatBuilder::Specifier&, const u8& value)
    {
        builder.push_integer(static_cast<u64>(value), IsNegative::No);
    }
};
template<>
struct Formatter<u16>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const u16& value)
    {
        builder.push_integer(static_cast<u64>(value), IsNegative::No);
    }
};
template<>
struct Formatter<u32>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const u32& value)
    {
        builder.push_integer(static_cast<u64>(value), IsNegative::No);
    }
};
template<>
struct Formatter<u64>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const u64& value)
    {
        builder.push_integer(static_cast<u64>(value), IsNegative::No);
    }
};

template<>
struct Formatter<i8>
{
    static void format(FormatBuilder& builder, const FormatBuilder::Specifier&, const i8& value)
    {
        const bool is_negative = value < 0;
        builder.push_integer(
            static_cast<u64>(is_negative ? -value : value),
            is_negative ? IsNegative::Yes : IsNegative::No
        );
    }
};
template<>
struct Formatter<i16>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const i16& value)
    {
        const bool is_negative = value < 0;
        builder.push_integer(
            static_cast<u64>(is_negative ? -value : value),
            is_negative ? IsNegative::Yes : IsNegative::No
        );
    }
};
template<>
struct Formatter<i32>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const i32& value)
    {
        const bool is_negative = value < 0;
        builder.push_integer(
            static_cast<u64>(is_negative ? -value : value),
            is_negative ? IsNegative::Yes : IsNegative::No
        );
    }
};
template<>
struct Formatter<i64>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const i64& value)
    {
        const bool is_negative = value < 0;
        builder.push_integer(
            static_cast<u64>(is_negative ? -value : value),
            is_negative ? IsNegative::Yes : IsNegative::No
        );
    }
};

template<>
struct Formatter<String>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const String& value)
    {
        builder.push_string(value.to_view());
    }
};
template<>
struct Formatter<StringView>
{
    static void
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const StringView& value)
    {
        builder.push_string(value);
    }
};

namespace Detail
{

inline bool format(FormatBuilder& builder)
{
    if (builder.consume_until_format_specifier())
    {
        // The string contains more format specifiers than the number of parameters passed.
        return false;
    }

    return true;
}

template<typename T, typename... Parameters>
inline bool format(FormatBuilder& builder, T parameter, Parameters&&... parameters)
{
    if (builder.consume_until_format_specifier())
    {
        FormatBuilder::Specifier specifier;
        if (!builder.consume_format_specifier(specifier))
        {
            // The format specifier is invalid.
            return false;
        }

        Formatter<T>::format(builder, specifier, parameter);
        return format(builder, forward<Parameters>(parameters)...);
    }

    // NOTE: If the number of parameters passed to the format function is greater than the
    // number of format specifiers in the string the function will still succeed.
    return true;
}

} // namespace Detail

template<typename... Parameters>
inline String format(StringView fmt, Parameters&&... parameters)
{
    FormatBuilder builder = FormatBuilder(fmt);
    if (Detail::format(builder, forward<Parameters>(parameters)...))
        return builder.release_as_string();

    // The string formatting process has failed, so an empty string is returned.
    return {};
}

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::format;
using AT::FormatBuilder;
using AT::Formatter;
#endif // AT_INCLUDE_GLOBALLY
