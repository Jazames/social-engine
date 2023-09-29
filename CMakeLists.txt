cmake_minimum_required(VERSION 3.12)

project(SocialEngine)

# Set the variables that need to be passed to the llamaCpp subproject
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(LLAMA_CLBLAST ON CACHE BOOL "Enable CLBlast for llamaCpp" FORCE)
#set(CMAKE_PREFIX_PATH "C:\\vcpkg\\packages\\clblast_x64-windows\\lib\\cmake\\CLBlast" CACHE STRING "Prefix path for CLBlast" FORCE)

# Include the subprojects
add_subdirectory(llamaCpp)
add_subdirectory(SocialEngine)
add_subdirectory(SocialEngineTest)
add_subdirectory(Run)

# If you need to enable testing for your project
enable_testing()
