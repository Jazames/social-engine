project(SocialEngine)

# Specify the source files
set(SOCIAL_ENGINE_SOURCES
    classifier.cpp
    responder.cpp
    social.cpp
    util.cpp
    embed.cpp
    # ... add other source files here ...
)

# Specify the header files
Set(SOCIAL_ENGINE_HEADERS
    classifier.h
    responder.h
    social.h
    util.h
    embed.h
)

# Create the SocialEngine library
add_library(SocialEngine ${SOCIAL_ENGINE_SOURCES} ${SOCIAL_ENGINE_HEADERS})

# Link it against llamaCpp
target_link_libraries(SocialEngine PUBLIC llama common)

# Include directories if there are any
target_include_directories(SocialEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
