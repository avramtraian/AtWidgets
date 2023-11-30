/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertions.h"
#include "AT/CoreDefines.h"
#include "AT/StringView.h"

namespace AT
{

class Error
{
    AT_MAKE_NONCOPYABLE(Error);

    template<typename T>
    friend class ErrorOr;

public:
    enum class Code : u32
    {
        Unknown = 0,
        OutOfMemory,
        IndexOutOfRange,
    };

    enum class Kind : u16
    {
        Code,
        String,
        UndefinedData,
    };

    NODISCARD AT_API static Error from_code(Code code);
    NODISCARD AT_API static Error from_string(StringView string);
    NODISCARD AT_API static Error from_undefined_data(u64 undefined_data);

private:
    Error() = default;
    AT_API Error(Error&& other) noexcept;
    Error& operator=(Error&& other) noexcept = delete;

private:
    Kind m_kind = Kind::Code;

    union
    {
        Code m_error_code = Code::Unknown;
        StringView m_error_string;
        u64 m_error_undefined_data;
    };
};

template<typename T>
class NODISCARD ErrorOr
{
    AT_MAKE_NONCOPYABLE(ErrorOr);
    AT_MAKE_NONMOVABLE(ErrorOr);

public:
    ALWAYS_INLINE ErrorOr(T value)
        : m_is_error(false)
        , m_value(move(value))
    {
    }

    ALWAYS_INLINE ErrorOr(Error&& error)
        : m_is_error(true)
        , m_error(move(error))
    {
    }

    ALWAYS_INLINE ErrorOr(Error::Code code)
        : m_is_error(true)
        , m_error(Error::from_code(code))
    {
    }

    ALWAYS_INLINE ~ErrorOr()
    {
        if (!m_is_error)
            m_value.~T();
        else
            m_error.~Error();
    }

public:
    NODISCARD ALWAYS_INLINE bool is_error() const { return m_is_error; }

    NODISCARD ALWAYS_INLINE T&& release_value() { return move(m_value); }
    NODISCARD ALWAYS_INLINE Error&& release_error() { return move(m_error); }

private:
    union
    {
        Error m_error;
        T m_value;
    };

    bool m_is_error;
};

template<>
class NODISCARD ErrorOr<void>
{
    AT_MAKE_NONCOPYABLE(ErrorOr);
    AT_MAKE_NONMOVABLE(ErrorOr);

public:
    ALWAYS_INLINE ErrorOr()
        : m_is_error(false)
    {
    }

    ALWAYS_INLINE ErrorOr(Error&& error)
        : m_is_error(true)
    {
        new (m_error_storage) Error(move(error));
    }

    ALWAYS_INLINE ErrorOr(Error::Code code)
        : m_is_error(true)
    {
        new (m_error_storage) Error(move(Error::from_code(code)));
    }

    ALWAYS_INLINE ~ErrorOr()
    {
        if (m_is_error)
            reinterpret_cast<Error*>(m_error_storage)->~Error();
    }

public:
    NODISCARD ALWAYS_INLINE bool is_error() const { return m_is_error; }

    ALWAYS_INLINE void release_value() {}
    NODISCARD ALWAYS_INLINE Error&& release_error() { return move(*reinterpret_cast<Error*>(m_error_storage)); }

private:
    alignas(Error) u8 m_error_storage[sizeof(Error)];
    bool m_is_error;
};

template<typename T>
class NODISCARD ErrorOr<T&>
{
    AT_MAKE_NONCOPYABLE(ErrorOr);
    AT_MAKE_NONMOVABLE(ErrorOr);

public:
    ALWAYS_INLINE ErrorOr(T& reference)
        : m_is_error(false)
    {
        m_pointer = &reference;
    }

    ALWAYS_INLINE ErrorOr(Error&& error)
        : m_is_error(true)
        , m_error(move(error))
    {
    }

    ALWAYS_INLINE ErrorOr(Error::Code code)
        : m_is_error(true)
        , m_error(Error::from_code(code))
    {
    }

    ALWAYS_INLINE ~ErrorOr()
    {
        if (m_is_error)
            m_error.~Error();
    }

public:
    NODISCARD ALWAYS_INLINE bool is_error() const { return m_is_error; }

    ALWAYS_INLINE T& release_value() { return *m_pointer; }
    NODISCARD ALWAYS_INLINE Error&& release_error() { return move(m_error); }

private:
    union
    {
        Error m_error;
        T* m_pointer;
    };

    bool m_is_error;
};

} // namespace AT

///
/// Checks if the result of the expression is an error and if so it will throw an assert.
/// The value that is stored in the expression result, if it is not an error, will be ignored.
///
/// This utility macro should only be used when actually handling errors is impossible or not useful. By throwing an
/// assertion, the application will crash.
///
#define MUST(expression)                                                 \
    {                                                                    \
        auto _at_error_or_##variable = (expression);                     \
        if (_at_error_or_##variable.is_error()) [[unlikely]]             \
        {                                                                \
            /* TODO: Log the expression that caused the must to fail. */ \
            INVALID_CODEPATH;                                            \
        }                                                                \
    }

///
/// Same behaviour and use cases as the plain MUST macro, except that the value that is stored in the expression result,
/// if it is not an error, will moved from the ErrorOr structure into the given variable.
/// The variable declaration can also be used to create a new variable and initialize it with the result value.
///
#define MUST_ASSIGN(variable_declaration, expression)                \
    auto&& CONCATENATE(_at_error_or, __LINE__) = (expression);       \
    if (CONCATENATE(_at_error_or, __LINE__).is_error()) [[unlikely]] \
    {                                                                \
        /* TODO: Log the expression that caused the must to fail. */ \
        INVALID_CODEPATH;                                            \
    }                                                                \
    variable_declaration = CONCATENATE(_at_error_or, __LINE__).release_value();

///
/// Checks if the result of the expression is an error and if so the function that called this macro will return.
/// The calling must function must return an ErrorOr. If the expression result is an error, it will be released and
/// returned, expecting to be handled by the next function in the callstack.
///
/// Unlike the MUST macro, this can be used to propagate errors though the system, as it will not crash the application.
///
#define TRY(expression)                           \
    {                                             \
        auto _at_error_or = (expression);         \
        if (_at_error_or.is_error()) [[unlikely]] \
        {                                         \
            return _at_error_or.release_error();  \
        }                                         \
        _at_error_or.release_value();             \
    }

///
/// Same behaviour and use cases as the plain TRY macro, except that the value that is stored in the expression result,
/// if it is not an error, will moved from the ErrorOr structure into the given variable.
/// The variable declaration can also be used to create a new variable and initialize it with the result value.
///
#define TRY_ASSIGN(variable_declaration, expression)                 \
    auto&& CONCATENATE(_at_error_or, __LINE__) = (expression);       \
    if (CONCATENATE(_at_error_or, __LINE__).is_error()) [[unlikely]] \
    {                                                                \
        return CONCATENATE(_at_error_or, __LINE__).release_error();  \
    }                                                                \
    variable_declaration = CONCATENATE(_at_error_or, __LINE__).release_value();

#if AT_INCLUDE_GLOBALLY
using AT::Error;
using AT::ErrorOr;
#endif // AT_INCLUDE_GLOBALLY
