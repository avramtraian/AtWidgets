# Copyright (c) 2023 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

function(add_widgets_library library_name macro_prefix)
    if (BUILD_AS_STATIC_LIBRARY)
        add_library(${library_name} STATIC ${ARGN})
    else ()
        add_library(${library_name} SHARED ${ARGN})
        target_compile_definitions(${library_name} PUBLIC "${macro_prefix}_SHARED_LIBRARY")
        target_compile_definitions(${library_name} PRIVATE "${macro_prefix}_BUILD_SHARED_LIBRARY")
    endif ()

    set_target_properties(${library_name} PROPERTIES PREFIX "ATW-")
    target_include_directories(${library_name} PUBLIC "${CMAKE_SOURCE_DIR}")
    target_include_directories(${library_name} PUBLIC "${CMAKE_SOURCE_DIR}/Libraries")

    add_dependencies(${library_name} AT)
    target_link_libraries(${library_name} PUBLIC AT)
endfunction()

add_subdirectory(Core)
add_subdirectory(Paint)
add_subdirectory(RenderAPI)
add_subdirectory(Rendering)
add_subdirectory(VulkanAPI)
