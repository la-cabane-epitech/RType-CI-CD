conan install . --build=missing \
    -s compiler.libcxx=libstdc++11 \
    -c tools.system.package_manager:mode=install \
    -c tools.system.package_manager:sudo=True

cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release