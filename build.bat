conan profile detect

conan install . --build=missing

cmake -S . -B ./build ^
    -G "Visual Studio 17 2022" ^
    -DCMAKE_TOOLCHAIN_FILE=build\generators\conan_toolchain.cmake ^
    -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release