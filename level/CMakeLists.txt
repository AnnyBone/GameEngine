# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(level)

file(GLOB LEVEL_SOURCE_FILES *.cpp *.c ../shared/SharedFormats.c)

add_executable(level ${LEVEL_SOURCE_FILES})

target_include_directories(level PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SYSTEM_INCLUDE_PATH} ${PROJECT_SOURCE_DIR}/level)

target_link_libraries(level platform dl pthread m)