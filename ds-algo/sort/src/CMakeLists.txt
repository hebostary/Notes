cmake_minimum_required (VERSION 2.8)

project(demo)

add_definitions(-std=c++11)

aux_source_directory(. DIR_SRCS)

add_subdirectory(common)
add_subdirectory(sort)

add_executable(demo ${DIR_SRCS})

target_link_libraries(demo common sort)
