#!/bin/bash

# TODO make this not erase everything
./james_build.sh


# Check if build directory exists
if [ -d "library_build" ]; then
    echo "Library Build directory exists. Deleting..."
    rm -rf library_build
    echo "Library Build directory deleted"
fi

# Create a new build directory and cd into it
mkdir library_build
cd library_build

# copy library files to library_build directory


# make this path relative and unix style C:\Users\James\source\repos\SocialEngine\build\SocialEngine\SocialEngine.dir\Release
cp ../build/SocialEngine/SocialEngine.dir/Release/SocialEngine.lib .
cp ../build/SocialEngine/SocialEngine.dir/Release/SocialEngine.lib ../../godot/modules/social_engine/libpath/SocialEngine.lib



