#!/bin/bash

conan install .

cmake --preset conan-release

cmake --build --preset conan-release

mv build/Release/rtype_server .
