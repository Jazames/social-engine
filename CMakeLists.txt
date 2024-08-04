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



# Set the variables that need to be passed to the llamaCpp subproject
#set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
#set(LLAMA_CLBLAST ON CACHE BOOL "Enable CLBlast for llamaCpp" FORCE)
#set(LLAMA_CUBLAS ON CACHE BOOL "Enable Cuda for llamaCpp" FORCE)
#set(CMAKE_PREFIX_PATH "C:\\vcpkg\\packages\\clblast_x64-windows\\lib\\cmake\\CLBlast" CACHE STRING "Prefix path for CLBlast" FORCE)

# Include the subprojects
add_subdirectory(llamaCpp)
add_subdirectory(SocialEngine)
add_subdirectory(SocialEngineTest)
add_subdirectory(Run)

# If you need to enable testing for your project
enable_testing()
