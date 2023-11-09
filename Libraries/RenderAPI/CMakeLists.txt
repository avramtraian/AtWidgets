# Copyright (c) 2023 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(RENDER_API_SOURCE_FILES
        Framebuffer.h
        Framebuffer.cpp
)

add_widgets_library(RenderAPI RENDER_API ${RENDER_API_SOURCE_FILES})
