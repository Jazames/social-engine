project(Run)

# Define sources
set(RUN_SOURCES
    main.cpp
)

add_executable(Run ${RUN_SOURCES})
target_link_libraries(Run SocialEngine)