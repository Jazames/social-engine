set(TARGET SocialEngine)


# Specify the source files
set(SOCIAL_ENGINE_SOURCES
    classifier.cpp
    lightweight_classifier.cpp
    responder.cpp
    social.cpp
    util.cpp
    llama_embed.cpp
    bert_embed.cpp
    global_knowledge.cpp
    # ... add other source files here ...
)
# Specify the header files
Set(SOCIAL_ENGINE_HEADERS
    classifier.h
    lightweight_classifier.h
    responder.h
    social.h
    util.h
    llama_embed.h
    bert_embed.h
    personality.h
    dialogue.h
    global_knowledge.h
)


add_library(${TARGET} OBJECT ${SOCIAL_ENGINE_SOURCES} ${SOCIAL_ENGINE_HEADERS})


if (BUILD_SHARED_LIBS)
    set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(${TARGET} PUBLIC .)
target_compile_features(${TARGET} PUBLIC cxx_std_11)
target_link_libraries(${TARGET} PRIVATE llama common bert)

## Specify the source files
#set(SOCIAL_ENGINE_SOURCES
#    classifier.cpp
#    responder.cpp
#    social.cpp
#    util.cpp
#    embed.cpp
#    # ... add other source files here ...
#)

## Specify the header files
#Set(SOCIAL_ENGINE_HEADERS
#    classifier.h
#    responder.h
#    social.h
#    util.h
#    embed.h
#)

# Create the SocialEngine library
# add_library(SocialEngine ${SOCIAL_ENGINE_SOURCES} ${SOCIAL_ENGINE_HEADERS})

# Link it against llamaCpp
#target_link_libraries(SocialEngine PUBLIC llama common)
#target_compile_features(SocialEngine PRIVATE cxx_std_11)

# Include directories if there are any
#target_include_directories(SocialEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
