rem 1. Crée le dossier build
mkdir build

rem 2. Détecte le profil Conan
conan profile detect

rem 3. Installe les dépendances avec Conan
conan install . --build=missing -if build

rem 4. Génère le projet avec CMake
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release

rem 5. Compile le projet
cmake --build . --config Release
