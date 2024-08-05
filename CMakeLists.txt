cmake_minimum_required(VERSION 3.14)

project(Social C CXX)

#
# Compile flags
#

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include(CheckCXXCompilerFlag)

# Include the subprojects
add_subdirectory(llamaCpp)
add_subdirectory(SocialEngine)
add_subdirectory(SocialEngineTest)
add_subdirectory(RunSocialEngine)

# If you need to enable testing for your project
enable_testing()
