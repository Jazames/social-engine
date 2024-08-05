set(TARGET SocialEngine)


# Specify the source files
set(SOCIAL_ENGINE_SOURCES
    bert_classifier.cpp
    responder.cpp
    social.cpp
    util.cpp
    bert_embed.cpp
    global_knowledge.cpp
    # ... add other source files here ...
)
# Specify the header files
Set(SOCIAL_ENGINE_HEADERS
    classifier.h
    bert_classifier.h
    responder.h
    social.h
    util.h
    bert_embed.h
    personality.h
    dialogue.h
    global_knowledge.h
    interaction.h
)


add_library(${TARGET} OBJECT ${SOCIAL_ENGINE_SOURCES} ${SOCIAL_ENGINE_HEADERS})


if (BUILD_SHARED_LIBS)
    set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(${TARGET} PUBLIC .)
target_compile_features(${TARGET} PUBLIC cxx_std_11)
target_link_libraries(${TARGET} PRIVATE llama common)

