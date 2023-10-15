project(SocialEngineTest)

# Where to find the FindGTest module
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#set(GTEST_SOURCE_DIR "C:\\vcpkg\\packages\\gtest_x64-windows\\share\\gtest")

# Find the GTest package
find_package(GTest CONFIG REQUIRED)

# Specify the test source files
set(SOCIAL_ENGINE_TEST_SOURCES
    insult_disposition_direction_test.cpp
    greeting_disposition_direction_test.cpp
    classifier_test.cpp
    pch.cpp
    test_util.cpp
    main.cpp
    # ... add other test files here ...
)

set(SOCIAL_ENGINE_TEST_HEADERS
    test_util.h
    pch.h
)

# Add the test executable
add_executable(SocialEngineTests ${SOCIAL_ENGINE_TEST_SOURCES} ${SOCIAL_ENGINE_TEST_HEADERS})

# Link the test executable against GTest libraries, SocialEngine, and potentially llamaCpp if needed directly
target_link_libraries(SocialEngineTests PRIVATE SocialEngine GTest::gtest GTest::gtest_main)

add_test(AllSocialEngineTests SocialEngineTests)
# Add the tests to CTest
#include(GoogleTest)
#gtest_discover_tests(SocialEngineTests)
