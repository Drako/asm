cmake_minimum_required(VERSION 3.16)
project(asm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.4
)

FetchContent_MakeAvailable(Catch2)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
