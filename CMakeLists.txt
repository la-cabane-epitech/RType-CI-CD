cmake_minimum_required(VERSION 3.10)
project(RType LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Important pour les .so
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --- Gestion Conan / OS ---
if (MSVC)
    include(${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake)
    add_compile_definitions(_WIN32_WINNT=0x0601 _CRT_SECURE_NO_WARNINGS)
else ()
    include(${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake)
endif()

# --- Packages ---
find_package(raylib REQUIRED CONFIG)
find_package(asio REQUIRED)

# --- 1. CREATION DE LA LIB NETWORK ---
# On regroupe tous les fichiers réseaux ici
add_library(rtype_network SHARED
    Src/Client/TCPClient.cpp
    Src/Client/UDPClient.cpp
    Src/Server/TCPServer.cpp
    Src/Server/UDPServer.cpp
)

target_include_directories(rtype_network PUBLIC ${CMAKE_SOURCE_DIR}/Include)

# Dépendances réseau (Asio + libs système Windows si besoin)
if (WIN32)
    target_link_libraries(rtype_network PUBLIC asio::asio ws2_32 mswsock)
else ()
    target_link_libraries(rtype_network PUBLIC asio::asio)
endif()

# --- 2. APPEL DES SOUS-DOSSIERS ---
add_subdirectory(Src/Server)
add_subdirectory(Src/Client)