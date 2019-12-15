cmake_minimum_required (VERSION 2.8)

project(demo)

aux_source_directory(. DIR_SRCS)

add_subdirectory(binarytree)

add_executable(demo ${DIR_SRCS})

target_link_libraries(demo BinaryTree)
