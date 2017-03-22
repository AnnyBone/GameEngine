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

project(engine)

file(GLOB ENGINE_SOURCE_FILES
        *.cpp *.c *.h
        client/*.cpp client/*.c client/*.h
        ../shared/SharedFormats.c
        )

add_library(engine SHARED ${ENGINE_SOURCE_FILES})

set_target_properties(engine PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set_target_properties(engine PROPERTIES PREFIX "")

target_compile_definitions(engine PUBLIC "-DENGINE_INTERNAL -DKATANA -Wno-write-strings")
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