rem 1. Crée le dossier build
mkdir build

rem 2. Détecte le profil Conan (ignore si déjà existant)
conan profile detect

rem 3. Installe les dépendances avec Conan dans build
conan install . --build=missing

rem 4. Génère le projet avec CMake en utilisant le toolchain Conan
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake

rem 5. Compile le projet
cmake --build . --config Release
