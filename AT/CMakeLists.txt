# Copyright (c) 2023 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(AT_SOURCE_FILES
        Assertions.cpp
        Assertions.h
        CoreDefines.h
        CoreTypes.h
        Error.cpp
        Error.h
        Format.cpp
        Format.h
        Log.cpp
        Log.h
        MemoryOperations.cpp
        MemoryOperations.h
        Pair.h
        Span.h
        String.cpp
        String.h
        StringView.cpp
        StringView.h
        Vector.h
)

if (BUILD_AS_STATIC_LIBRARY)
    add_library(AT STATIC ${AT_SOURCE_FILES})
else ()
    add_library(AT SHARED ${AT_SOURCE_FILES})
    target_compile_definitions(AT PUBLIC "AT_SHARED_LIBRARY")
    target_compile_definitions(AT PRIVATE "AT_BUILD_SHARED_LIBRARY")
endif ()

set_target_properties(AT PROPERTIES OUTPUT_NAME "AT-Framework")
target_include_directories(AT PUBLIC "${CMAKE_SOURCE_DIR}")
