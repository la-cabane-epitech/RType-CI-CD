cmake_minimum_required(VERSION 3.10)
project(RType LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (MSVC)
    include(${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake)
    add_compile_definitions(_WIN32_WINNT=0x0601 _CRT_SECURE_NO_WARNINGS)
else ()
    include(${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake)
endif()

find_package(raylib REQUIRED CONFIG)
find_package(asio REQUIRED)

add_subdirectory(Src/Network)
add_subdirectory(Src/Server)
add_subdirectory(Src/Client)