#cmake_minimum_required (VERSION 2.8)
cmake_minimum_required(VERSION 2.8...3.13)

project(cpp_learn)

add_definitions(-std=c++17)

aux_source_directory(. DIR_SRCS)

add_subdirectory(common)
add_subdirectory(std)

add_executable(cpp.bin ${DIR_SRCS})

#在Linux平台上，依赖其他库的库一定要放到被依赖库的前面，否则链接时会出现找不到符号定义（undefined reference to）的错误
target_link_libraries(cpp.bin std common uuid)

#MAC平台上，可以调换依赖库和被依赖库顺序
#target_link_libraries(demo common std uuid)
