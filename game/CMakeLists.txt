#[[
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
]]

project(game)

set(GAME_PROJECT openkatana CACHE STRING "Game project selected for compilation")
set(GAME_PROJECT_OPTIONS openkatana;adamas;ictus;planetshift)
set_property(CACHE GAME_PROJECT PROPERTY STRINGS ${GAME_PROJECT_OPTIONS})
message(STATUS GAME_PROJECT='${GAME_PROJECT}')

file(GLOB GAME_SOURCE_FILES
        *.cpp *.c *.h
        server/*.cpp server/*.c server/*.h
        client/*.cpp client/*.c client/*.h
        server/${GAME_PROJECT}/*.cpp server/${GAME_PROJECT}/*.c server/${GAME_PROJECT}/*.h
        client/${GAME_PROJECT}/*.cpp client/${GAME_PROJECT}/*.c client/${GAME_PROJECT}/*.h
        )

add_library(game MODULE ${GAME_SOURCE_FILES})

set_target_properties(game PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${GAME_PROJECT}/)
set_target_properties(game PROPERTIES PREFIX "")

if(${GAME_PROJECT} MATCHES openkatana)
    target_compile_definitions(game PUBLIC "-DGAME_OPENKATANA -DGAME_DECAY")
elseif(${GAME_PROJECT} MATCHES adamas)
    target_compile_definitions(game PUBLIC -DGAME_ADAMAS)
elseif(${GAME_PROJECT} MATCHES ictus)
    target_compile_definitions(game PUBLIC -DGAME_ICTUS)
endif()

target_include_directories(game PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SYSTEM_INCLUDE_PATH}
        server/ server/${GAME_PROJECT}
        client/ client/${GAME_PROJECT}
        )
target_link_libraries(game platform dl)