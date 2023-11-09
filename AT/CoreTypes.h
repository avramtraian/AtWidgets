    /*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"
#include <stdint.h>
#include <type_traits>

#define AT_MAKE_NONCOPYABLE(type) \
    type(const type&) = delete;   \
    type& operator=(const type&) = delete;

#define AT_MAKE_NONMOVABLE(type)    \
    type(type&&) noexcept = delete; \
    type& operator=(type&&) noexcept = delete;

namespace AT
{

//=============================================================================
// Primitive data types.
//=============================================================================

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

#if AT_ARCHITECTURE_32_BIT
using usize = u32;
using ssize = i32;
using uintptr = u32;
#elif AT_ARCHITECTURE_64_BIT
using usize = u64;
using ssize = i64;
using uintptr = u64;
#endif // Architectures.

using ReadonlyByte = const u8;
using WriteonlyByte = u8;
using ReadWriteByte = u8;

using ReadonlyBytes = ReadonlyByte*;
using WriteonlyBytes = WriteonlyByte*;
using ReadWriteBytes = ReadWriteByte*;

constexpr usize InvalidSize = static_cast<usize>(-1);

//=============================================================================
// Type manipulation utilities.
//=============================================================================

namespace Detail
{

template<typename T>
struct Identity
{
    using Type = T;
};

template<typename T>
struct RemoveConst
{
    using Type = T;
};
template<typename T>
struct RemoveConst<const T>
{
    using Type = T;
};

template<typename T>
struct RemovePointer
{
    using Type = T;
};
template<typename T>
struct RemovePointer<T*>
{
    using Type = T;
};
template<typename T>
struct RemovePointer<const T*>
{
    using Type = T;
};
template<typename T>
struct RemovePointer<volatile T*>
{
    using Type = T;
};
template<typename T>
struct RemovePointer<const volatile T*>
{
    using Type = T;
};

template<typename T>
struct RemoveReference
{
    using Type = T;
};
template<typename T>
struct RemoveReference<T&>
{
    using Type = T;
};
template<typename T>
struct RemoveReference<T&&>
{
    using Type = T;
};

template<typename T>
struct RemoveVolatile
{
    using Type = T;
};
template<typename T>
struct RemoveVolatile<volatile T>
{
    using Type = T;
};

} // namespace Detail

template<typename T>
using Identity = typename Detail::Identity<T>::Type;
template<typename T>
using RemoveConst = typename Detail::RemoveConst<T>::Type;
template<typename T>
using RemovePointer = typename Detail::RemovePointer<T>::Type;
template<typename T>
using RemoveReference = typename Detail::RemoveReference<T>::Type;
template<typename T>
using RemoveVolatile = typename Detail::RemoveVolatile<T>::Type;

template<typename T>
using RemoveCV = RemoveConst<RemoveVolatile<T>>;
template<typename T>
using RemoveCVR = RemoveCV<RemoveReference<T>>;

//=============================================================================
// Move semantics utilities.
//=============================================================================

template<typename T>
NODISCARD ALWAYS_INLINE constexpr RemoveReference<T>&& move(T&& instance)
{
    return static_cast<RemoveReference<T>&&>(instance);
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>& instance)
{
    return static_cast<T&&>(instance);
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>&& instance)
{
    return static_cast<T&&>(instance);
}

//=============================================================================
// Constant boolean expressions.
//=============================================================================

namespace Detail
{

template<typename T>
static constexpr bool IsConst = false;
template<typename T>
static constexpr bool IsConst<const T> = true;

template<typename T>
static constexpr bool IsFloatingPoint = false;
template<>
static constexpr bool IsFloatingPoint<f32> = true;
template<>
static constexpr bool IsFloatingPoint<f64> = true;

template<typename T>
static constexpr bool IsPointer = false;
template<typename T>
static constexpr bool IsPointer<T*> = true;

template<typename T>
static constexpr bool IsReference = false;
template<typename T>
static constexpr bool IsReference<T&> = true;
template<typename T>
static constexpr bool IsReference<const T&> = true;
template<typename T>
static constexpr bool IsReference<T&&> = true;
template<typename T>
static constexpr bool IsReference<const T&&> = true;

template<typename T>
static constexpr bool IsSigned = false;
template<>
static constexpr bool IsSigned<i8> = true;
template<>
static constexpr bool IsSigned<i16> = true;
template<>
static constexpr bool IsSigned<i32> = true;
template<>
static constexpr bool IsSigned<i64> = true;

template<typename T>
static constexpr bool IsUnsigned = false;
template<>
static constexpr bool IsUnsigned<u8> = true;
template<>
static constexpr bool IsUnsigned<u16> = true;
template<>
static constexpr bool IsUnsigned<u32> = true;
template<>
static constexpr bool IsUnsigned<u64> = true;

template<typename T>
static constexpr bool IsVoid = false;
template<>
static constexpr bool IsVoid<void> = true;

template<typename T>
static constexpr bool IsVolatile = false;
template<typename T>
static constexpr bool IsVolatile<volatile T> = true;

} // namespace Detail

template<typename T>
static constexpr bool IsConst = Detail::IsConst<T>;
template<typename T>
static constexpr bool IsInteger = Detail::IsSigned<T> || Detail::IsUnsigned<T>;
template<typename T>
static constexpr bool IsFloatingPoint = Detail::IsFloatingPoint<T>;
template<typename T>
static constexpr bool IsPointer = Detail::IsPointer<T>;
template<typename T>
static constexpr bool IsReference = Detail::IsReference<T>;
template<typename T>
static constexpr bool IsSigned = Detail::IsSigned<T>;
template<typename T>
static constexpr bool IsUnsigned = Detail::IsUnsigned<T>;
template<typename T>
static constexpr bool IsVoid = Detail::IsVoid<T>;
template<typename T>
static constexpr bool IsVolatile = Detail::IsVolatile<T>;

template<typename T, typename Q>
static constexpr bool IsSame = std::is_same_v<T, Q>;

//=============================================================================
// Type traits.
//=============================================================================

template<typename T>
struct TypeTraits
{
    NODISCARD ALWAYS_INLINE static u64 get_hash(const T&) { return 0; }
};

template<>
struct TypeTraits<i32>
{
    NODISCARD ALWAYS_INLINE static u64 get_hash(const u32& value) { return value; }
};

} // namespace AT

#if AT_INCLUDE_GLOBALLY
using AT::f32;
using AT::f64;
using AT::forward;
using AT::i16;
using AT::i32;
using AT::i64;
using AT::i8;
using AT::move;
using AT::ReadonlyByte;
using AT::ReadonlyBytes;
using AT::ReadWriteByte;
using AT::ReadWriteBytes;
using AT::RemoveConst;
using AT::RemoveReference;
using AT::ssize;
using AT::u16;
using AT::u32;
using AT::u64;
using AT::u8;
using AT::uintptr;
using AT::usize;
using AT::WriteonlyByte;
using AT::WriteonlyBytes;
#endif // AT_INCLUDE_GLOBALLY
