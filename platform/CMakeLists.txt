

include_directories(include)
file(
    GLOB
    PLATFORM_SOURCE_FILES
    "*.cpp"
    "*.c"
)

add_library(platform STATIC ${PLATFORM_SOURCE_FILES})
add_definitions("-D_DEBUG")

target_include_directories(platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SYSTEM_INCLUDE_PATH})
target_link_libraries(platform X11_LIBRARY)