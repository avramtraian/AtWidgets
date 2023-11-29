/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Error.h"

namespace AT
{

Error Error::from_code(Code code)
{
    Error error = {};
    error.m_kind = Kind::Code;
    error.m_error_code = code;
    return error;
}

Error Error::from_string(StringView string)
{
    Error error;
    error.m_kind = Kind::String;
    error.m_error_string = string;
    return error;
}

Error Error::from_undefined_data(AT::u64 undefined_data)
{
    Error error;
    error.m_kind = Kind::UndefinedData;
    error.m_error_undefined_data = undefined_data;
    return error;
}

Error::Error(Error&& other) noexcept
    : m_kind(other.m_kind)
{
    switch (m_kind)
    {
        case Kind::Code:
            m_error_code = other.m_error_code;
            other.m_error_code = Code::Unknown;
            break;

        case Kind::String:
            m_error_string = move(other.m_error_string);
            other.m_error_string = {};
            break;

        case Kind::UndefinedData:
            m_error_undefined_data = other.m_error_undefined_data;
            other.m_error_undefined_data = 0;
            break;
    }
}

} // namespace AT
