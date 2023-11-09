# Copyright (c) 2023 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(RENDERING_SOURCE_FILES
        Renderer.cpp
        Renderer.h
)

add_widgets_library(Rendering RENDERING ${RENDERING_SOURCE_FILES})
add_dependencies(Rendering RenderAPI)
