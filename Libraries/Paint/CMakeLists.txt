# Copyright (c) 2023 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

set(PAINT_SOURCE_FILES
        Painter.h
        Painter.cpp
)

add_widgets_library(Paint PAINT ${PAINT_SOURCE_FILES})
