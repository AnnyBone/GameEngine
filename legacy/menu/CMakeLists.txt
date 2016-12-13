# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(menu)

file(GLOB MENU_SOURCE_FILES *.cpp *.c)

add_library(menu MODULE ${MENU_SOURCE_FILES})

set_target_properties(menu PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/game/)
set_target_properties(menu PROPERTIES PREFIX "")

target_include_directories(menu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SYSTEM_INCLUDE_PATH})
target_link_libraries(menu platform dl)