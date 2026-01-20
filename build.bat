conan profile detect

rem Create the build directory if it does not exist
If not exist build\ (mkdir build)

rem Install dependencies with Conan in build
cd build
conan install .. --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
cd ..

rem Generate the project with CMake using the Conan toolchain and CMP0091=NEW
cmake -B ./build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=build/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW

rem Compile the project
cmake --build ./build --config Release --target rtype_client rtype_server

copy build\Src\Client\Release\rtype_client.exe .
copy build\Src\Server\Release\rtype_server.exe .
copy build\Src\Network\Release\rtype_network.dll .
