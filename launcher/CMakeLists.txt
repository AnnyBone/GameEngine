# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(launcher)

file(GLOB LAUNCHER_SOURCE_FILES *.cpp *.c)

add_executable(launcher ${LAUNCHER_SOURCE_FILES})

target_include_directories(launcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SYSTEM_INCLUDE_PATH})
target_link_libraries(launcher platform X11 dl engine)