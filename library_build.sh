#!/bin/bash

# TODO make this not erase everything

cd llamaCpp

if [ -d "build" ]; then
    echo "Build directory exists. Deleting..."
    rm -rf build
    echo "Build directory deleted"
fi

mkdir build
cd build

echo "Generating build files"
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=/c/vcpkg/scripts/buildsystems/vcpkg.cmake -DLLAMA_CUBLAS=ON -DBUILD_SHARED_LIBS=OFF

echo "Building"
cmake --build . --config Release

echo "Installing to C:/LlamaCpp"
cmake --install . --prefix C:/LlamaCpp



# Check if build directory exists
#if [ -d "library_build" ]; then
#    echo "Library Build directory exists. Deleting..."
#    rm -rf library_build
#    echo "Library Build directory deleted"
#fi
#
## Create a new build directory and cd into it
#mkdir library_build
#cd library_build
#
## copy library files to library_build directory
#
#
## make this path relative and unix style C:\Users\James\source\repos\SocialEngine\build\SocialEngine\SocialEngine.dir\Release
#cp ../build/SocialEngine/SocialEngine.dir/Release/SocialEngine.lib .
#cp ../build/SocialEngine/SocialEngine.dir/Release/SocialEngine.lib ../../godot/modules/social_engine/libpath/SocialEngine.lib



