# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

cmake_minimum_required(VERSION 3.5.1)
project(XENON)

# Set all of our output directories.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/lib/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

option(XENON_COMPILE_TOOLS "Compile Xenon utilities" ON)

include_directories("${CMAKE_SOURCE_DIR}/include/")           # Base include directories.
include_directories("${CMAKE_SOURCE_DIR}/platform/include/")  # Platform lib headers get used for everything.
include_directories("${CMAKE_SOURCE_DIR}/shared/")            # Xenon shared headers.

add_definitions("-std=c++11" "-D_DEBUG")
add_subdirectory(platform)
add_subdirectory(menu)
add_subdirectory(game)
add_subdirectory(engine)
add_subdirectory(launcher)

if(XENON_COMPILE_TOOLS)
    add_subdirectory(level)
    add_subdirectory(editor)
endif(XENON_COMPILE_TOOLS)
