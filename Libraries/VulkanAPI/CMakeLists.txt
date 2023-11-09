# Copyright (c) 2023 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(VULKAN_API_SOURCE_FILES
        Context.cpp
        Context.h
)

add_widgets_library(VulkanAPI VULKAN_API ${VULKAN_API_SOURCE_FILES})
add_dependencies(VulkanAPI RenderAPI)
