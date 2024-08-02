set(TARGET SocialEngineTest)

find_package(GTest CONFIG REQUIRED)

# Specify the test source files
set(SOCIAL_ENGINE_TEST_SOURCES
    greeting_disposition_direction_test.cpp
    insult_disposition_direction_test.cpp
    question_disposition_direction_test.cpp
    statement_disposition_direction_test.cpp
    request_disposition_direction_test.cpp
    compliment_disposition_direction_test.cpp
    joke_disposition_direction_test.cpp
    salutation_disposition_direction_test.cpp
    embedding_test.cpp
    classifier_test.cpp
    response_test.cpp
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
add_executable(${TARGET} ${SOCIAL_ENGINE_TEST_SOURCES} ${SOCIAL_ENGINE_TEST_HEADERS})
install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE SocialEngine llama common GTest::gtest GTest::gtest_main)
target_compile_features(${TARGET} PRIVATE cxx_std_11)
if(TARGET BUILD_INFO)
    add_dependencies(${TARGET} BUILD_INFO)
endif()






## Find the GTest package
#find_package(GTest CONFIG REQUIRED)
#
## Specify the test source files
#set(SOCIAL_ENGINE_TEST_SOURCES
#    insult_disposition_direction_test.cpp
#    greeting_disposition_direction_test.cpp
#    classifier_test.cpp
#    pch.cpp
#    test_util.cpp
#    main.cpp
#    # ... add other test files here ...
#)
#
#set(SOCIAL_ENGINE_TEST_HEADERS
#    test_util.h
#    pch.h
#)
#
## Add the test executable
#add_executable(SocialEngineTests ${SOCIAL_ENGINE_TEST_SOURCES} ${SOCIAL_ENGINE_TEST_HEADERS})
#
## Link the test executable against GTest libraries, SocialEngine, and potentially llamaCpp if needed directly
#target_link_libraries(SocialEngineTests PRIVATE SocialEngine GTest::gtest GTest::gtest_main)
#
#add_test(AllSocialEngineTests SocialEngineTests)
## Add the tests to CTest
##include(GoogleTest)
##gtest_discover_tests(SocialEngineTests)
#