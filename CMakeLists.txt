# Copyright (c) 2023 Traian Avram. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause.

cmake_minimum_required(VERSION 3.10)
project(AtWidgets CXX C)

#---------------------------------------------------------------
# Project options.
#---------------------------------------------------------------

option(BUILD_AS_STATIC_LIBRARY "Compile the widgets framework as a static library." OFF)

# Set the project global C++ configuration.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build")

# Make the CMake predefined targets appear nicer if using the Visual Studio toolchain.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

#---------------------------------------------------------------
# Project subdirectories.
#---------------------------------------------------------------

# The framework that implements custom templated containers, algorithms and various data structures.
add_subdirectory(AT)

# The modules that compose the AtWidgets SDK and that the end-user links against.
add_subdirectory(Libraries)
