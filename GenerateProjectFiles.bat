:: Copyright (c) 2023 Traian Avram. All rights reserved.
:: SPDX-License-Identifier: BSD-3-Clause.

@echo off
pushd "%~dp0"

:: Create the Build directory if it doesn't already exist.
if not exist "Build"/ ( mkdir "Build" )
pushd "Build"

:: Create the Intermediate directory if it doesn't already exist.
if not exist "Intermediate/" ( mkdir "Intermediate" )
pushd "Intermediate"

if exist "CMakeCache.txt" ( del "CMakeCache.txt" )
if exist "CMakeFiles" ( rd /S /Q "CMakeFiles" )

cmake -B ./ -S ../../ -G "Visual Studio 17 2022" -DCMAKE_CXX_COMPILER=clang

popd
popd

popd
pause
