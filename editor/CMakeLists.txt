# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(editor)

file(GLOB EDITOR_SOURCE_FILES *.cpp *.c)

add_executable(editor ${EDITOR_SOURCE_FILES})

find_package(wxWidgets COMPONENTS core base gl REQUIRED)

target_include_directories(editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SYSTEM_INCLUDE_PATH} ${wxWidgets_USE_FILE})
target_link_libraries(editor platform X11 dl engine ${wxWidgets_LIBRARIES})