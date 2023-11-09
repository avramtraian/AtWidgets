/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

// Check if the platform is Windows.
#ifdef _WIN32
    #define AT_PLATFORM_WINDOWS 1

    #ifdef _WIN64
        #define AT_ARCHITECTURE_32_BIT 0
        #define AT_ARCHITECTURE_64_BIT 1
    #else
        #define AT_ARCHITECTURE_32_BIT 1
        #define AT_ARCHITECTURE_64_BIT 0
    #endif // _WIN64
#else
    #define AT_PLATFORM_WINDOWS    0
    #define AT_ARCHITECTURE_32_BIT 0
    #define AT_ARCHITECTURE_64_BIT 0
#endif // _WIN32

// If none of the supported platforms are detected don't bother trying to build.
#if !AT_PLATFORM_WINDOWS
    #error Unsupported/Unknown platform!
#endif // Any platform.

// Check if the compiler is Clang.
#ifdef __clang__
    #define AT_COMPILER_CLANG 1
#else
    #define AT_COMPILER_CLANG 0
#endif // __clang__

// Check if the compiler is GCC.
// NOTE: Clang might define __GNUC__ too.
#if defined(__GNUC__) && !AT_COMPILER_CLANG
    #define AT_COMPILER_GCC 1
#else
    #define AT_COMPILER_GCC 0
#endif // defined(__GNUC__) && !AT_COMPILER_CLANG

// Check if the compiler is MSVC.
// NOTE: Clang might define _MSC_BUILD too.
#if defined(_MSC_BUILD) && !AT_COMPILER_CLANG
    #define AT_COMPILER_MSVC 1
#else
    #define AT_COMPILER_MSVC 0
#endif // _MSC_BUILD && !AT_COMPILER_CLANG

// If none of the supported compilers are detected don't bother trying to build.
#if !AT_COMPILER_CLANG && !AT_COMPILER_GCC && !AT_COMPILER_MSVC
    #error Unsupported/Unknown compiler!
#endif // Any compiler.

#ifdef AT_SHARED_LIBRARY
    #if AT_COMPILER_MSVC
        #ifdef AT_BUILD_SHARED_LIBRARY
            #define AT_API __declspec(dllexport)
        #else
            #define AT_API __declspec(dllimport)
        #endif // AT_BUILD_SHARED_LIBRARY
    #else
        #define AT_API __attribute__((visibility("default")))
    #endif // AT_COMPILER_MSVC
#else
    #define AT_API // Define as nothing.
#endif // AT_SHARED_LIBRARY

#if AT_COMPILER_MSVC
    #define ALWAYS_INLINE __forceinline
    #define AT_FUNCTION   __FUNCTION__
    #define AT_DEBUGBREAK __debugbreak()
#else
    #define ALWAYS_INLINE __attribute__((always_inline))
    #define AT_FUNCTION   __PRETTY_FUNCTION__
    #define AT_DEBUGBREAK __builtin_trap()
#endif // Compilers.

#define NODISCARD    [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]
#define AT_DEPRECATED
#define AT_DANGEROUS

#define AT_INCLUDE_GLOBALLY 1
