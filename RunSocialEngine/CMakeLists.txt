set(TARGET RunSocialEngine)

# Define sources
set(RUN_SOURCES
    main.cpp
)

add_executable(${TARGET} ${RUN_SOURCES})
install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE SocialEngine llama common)
target_compile_features(${TARGET} PRIVATE cxx_std_11)
if(TARGET BUILD_INFO)
    add_dependencies(${TARGET} BUILD_INFO)
endif()