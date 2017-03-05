# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(engine)

file(GLOB ENGINE_SOURCE_FILES
        *.cpp *.c
        client/*.cpp client/*.c
        ../shared/SharedFormats.c)

add_library(engine SHARED ${ENGINE_SOURCE_FILES})

set_target_properties(engine PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set_target_properties(engine PROPERTIES PREFIX "")

target_compile_definitions(engine PUBLIC "-DKATANA -Wno-write-strings")
target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SYSTEM_INCLUDE_PATH}
        client/
        ../)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(engine "-framework OpenGL")
else()
    target_link_libraries(engine GL GLEW)
endif()
target_link_libraries(engine platform dl openal SDL2-2.0)