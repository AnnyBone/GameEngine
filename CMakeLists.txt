cmake_minimum_required(VERSION 3.5.1)
project(XENON)

set(CMAKE_BINARY_DIR bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_definitions("-std=c++11")
add_subdirectory(platform)