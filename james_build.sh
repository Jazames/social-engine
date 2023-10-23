#!/bin/bash

# Check if build directory exists
if [ -d "build" ]; then
    echo "Build directory exists. Deleting..."
    rm -rf build
    echo "Build directory deleted"
fi

# Create a new build directory and cd into it
mkdir build
cd build

# Run the cmake command
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=/c/vcpkg/scripts/buildsystems/vcpkg.cmake -DLLAMA_CUBLAS=ON -DBUILD_SHARED_LIBS=OFF

echo "Building"
cmake --build . --config Release
cmake --build . --config Debug

echo "Completed!"
