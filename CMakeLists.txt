cmake_minimum_required(VERSION 3.10)
project(acadia_interview)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "- O0 −Wall −ansi −Wpedantic −Wextra")
add_subdirectory(tests)
add_executable(b-twe main.cpp Objects.h)
