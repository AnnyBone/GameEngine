# Copyright (C) 2011-2016 Mark E Sowden
# Public Domain, do what you like. A pirate is you.

project(fxproto)

file(GLOB FXPROTO_SOURCE_FILES "*.cpp")

add_executable(fxproto ${FXPROTO_SOURCE_FILES})

target_include_directories(fxproto PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SYSTEM_INCLUDE_PATH})

target_link_libraries(fxproto platform FOX-1.6)