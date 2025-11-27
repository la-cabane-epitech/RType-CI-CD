cmake_minimum_required(VERSION 3.20)

project(rtype_ecs LANGUAGES CXX)

# Utiliser C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Ajouter l'exécutable
add_executable(rtype_ecs
    src/main.cpp
)

# -----------------------------------------------------------------------
# Solution: Lien manuel de Raylib et de TOUTES ses dépendances Linux
# On suppose ici que la librairie 'libraylib.so' est dans un chemin standard
# -----------------------------------------------------------------------
target_link_libraries(rtype_ecs 
    raylib 
    GL 
    m 
    pthread 
    dl 
    rt 
    X11
)
