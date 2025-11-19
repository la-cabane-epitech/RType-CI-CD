cmake_minimum_required(VERSION 3.10)
project(RType LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --- Executable ---
add_executable(rtype_server src/server/main.cpp)
add_executable(rtype_client src/client/main.cpp)

# --- CLEAN / FCLEAN / RE (compatible) ---
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning build files..."
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMENT "CLEAN_BUILD"
)

add_custom_target(fclean
    COMMAND ${CMAKE_COMMAND} -E echo "Full clean deleting build directory..."
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "FCLEAN"
)

add_custom_target(re
    COMMAND ${CMAKE_COMMAND} -E echo "Rebuilding..."
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} ${CMAKE_COMMAND} ..
    COMMENT "REBUILD"
)
