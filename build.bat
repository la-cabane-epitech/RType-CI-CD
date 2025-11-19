conan profile detect

conan install . --build=missing

dir build\generators

conan install . --build=missing  -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True

cmake --build build --config Release