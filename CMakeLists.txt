cmake_minimum_required(VERSION 3.10)
project(RType LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Lier le runtime C++ statiquement sur MinGW ---
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

# --- Executable ---
add_executable(rtype_server src/server/main.cpp)
# add_executable(rtype_client src/client/main.cpp)

# --- Copier les DLL tierces pour Windows (exemple SDL2) ---
if(WIN32)
    set(SDL2_DLL_PATH "C:/path/to/SDL2/bin")  # <-- adapte selon ton installation
    add_custom_command(TARGET rtype_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL2_DLL_PATH}/SDL2.dll"
            $<TARGET_FILE_DIR:rtype_server>
    )

    # Ajouter d'autres DLL si nécessaire, ex:
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different "C:/path/to/SDL2_image.dll" $<TARGET_FILE_DIR:rtype_server>
endif()

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
