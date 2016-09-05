# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(modelviewer)

file(GLOB MODELVIEW_SOURCE_FILES "*.cpp")

add_executable(modelviewer ${MODELVIEW_SOURCE_FILES})

target_include_directories(modelviewer PUBLIC ../ModelViewer ${CMAKE_SYSTEM_INCLUDE_PATH})

target_link_libraries(modelviewer platform FOX-1.6)