# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

cmake_minimum_required(VERSION 3.5.1)
project(XENON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/")

add_definitions("-std=c++11")
add_subdirectory(platform)