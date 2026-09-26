cmake_minimum_required(VERSION 3.28)

project(Myrias LANGUAGES CXX)

# CMake doesn't map C++26 for MSVC yet: 23 there becomes /std:c++latest (which includes the
# C++26 features MSVC implements). Other compilers get C++26 directly.
if(MSVC)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 26)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MYRIAS_BUILD_TESTS "Build the GoogleTest suite" ON)
option(MYRIAS_BUILD_BENCHMARKS "Build the Google Benchmark suite" OFF)

# SFML 3 prebuilt in libs/SFML, linked statically. Its static libs use the dynamic CRT (/MD),
# which is CMake's default, so the runtime is left untouched.
set(SFML_STATIC_LIBRARIES ON)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System
             CONFIG PATHS "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML/lib/cmake/SFML" NO_DEFAULT_PATH)

# Common warnings for every first-party target.
# clang-cl (CXX_COMPILER_ID=Clang, CXX_COMPILER_FRONTEND_VARIANT=MSVC) takes MSVC-style flags,
# not GNU-style ones, so branch on the frontend variant rather than the compiler id.
add_library(MyriasWarnings INTERFACE)
target_compile_options(MyriasWarnings INTERFACE
    $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/W4 /permissive- /Zc:__cplusplus /utf-8>
    $<$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>:-Wall -Wextra -Wpedantic>)

add_subdirectory(src/Engine)
add_subdirectory(src/Game)

if(MYRIAS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(MYRIAS_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
