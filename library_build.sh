#!/bin/bash


##Adjust the llama cmakelists file.
#echo "Modifying llamaCpp CMakeLists"
#
#
## Define the file path
#FILE="llamaCpp/CMakeLists.txt"
#
## Check if the file exists
#if [ ! -f "$FILE" ]; then
#    echo "Error: File $FILE does not exist."
#    exit 1
#fi
#
## Create a temporary file
#TEMP_FILE=$(mktemp)
#
## Use awk to edit the file
#awk '
#    /cmake_minimum_required\(/ {
#        print
#        print "cmake_policy(SET CMP0091 NEW)"
#        next
#    }
#    /set\(CMAKE_C_STANDARD_REQUIRED true\)/ {
#        print
#        print "set(CMAKE_MSVC_RUNTIME_LIBRARY \"MultiThreaded\$<\$<CONFIG:Debug>:Debug>\")"
#        next
#    }
#    { print }
#' "$FILE" > "$TEMP_FILE"
#
## Replace original file with the modified one
#mv "$TEMP_FILE" "$FILE"
#
#echo "Done modifying"


#build llama
cd llamaCpp

if [ -d "build" ]; then
    echo "Build directory exists. Deleting..."
    rm -rf build
    echo "Build directory deleted"
fi

mkdir build
cd build

echo "Generating build files"
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=/c/vcpkg/scripts/buildsystems/vcpkg.cmake -DLLAMA_CUBLAS=ON -DBUILD_SHARED_LIBS=OFF -DLLAMA_STATIC=ON

echo "Building"
cmake --build . --config Release

echo "Installing to C:/LlamaCpp"
cmake --install . --prefix C:/LlamaCpp

echo "Cleaning up"

cd ..
git reset --hard

echo "Finished"



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



