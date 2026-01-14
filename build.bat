conan profile detect

rem Crée le dossier build si absent
If not exist build\ (mkdir build)

rem Installe les dépendances avec Conan dans build
cd build
conan install .. --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
cd ..

rem Génère le projet avec CMake en utilisant le toolchain Conan et CMP0091=NEW
cmake -B ./build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=build/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW

rem Compile le projet
cmake --build ./build --config Release --target rtype_client rtype_server
