# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(game)

set(GAME_PROJECT openkatana CACHE STRING "Game project selected for compilation")
set(GAME_PROJECT_OPTIONS openkatana;adamas;ictus;planetshift)
set_property(CACHE GAME_PROJECT PROPERTY STRINGS ${GAME_PROJECT_OPTIONS})
message(STATUS GAME_PROJECT='${GAME_PROJECT}')

file(GLOB GAME_SOURCE_FILES
        *.cpp *.c
        server/*.cpp server/*.c
        client/*.cpp client/*.c
        server/${GAME_PROJECT}/*.cpp server/${GAME_PROJECT}/*.c
        client/${GAME_PROJECT}/*.cpp client/${GAME_PROJECT}/*.c)

add_library(game MODULE ${GAME_SOURCE_FILES})

set_target_properties(game PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/game/)
set_target_properties(game PROPERTIES PREFIX "")

if(${GAME_PROJECT} MATCHES openkatana)
    target_compile_definitions(game PUBLIC "-DGAME_OPENKATANA -DGAME_DECAY")
endif()

target_include_directories(game PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SYSTEM_INCLUDE_PATH}
        server/ server/${GAME_PROJECT}
        client/ client/${GAME_PROJECT})
target_link_libraries(game platform dl)