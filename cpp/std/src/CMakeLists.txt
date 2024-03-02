#cmake_minimum_required (VERSION 2.8)
cmake_minimum_required(VERSION 2.8...3.13)

project(demo)

add_definitions(-std=c++11)

aux_source_directory(. DIR_SRCS)

add_subdirectory(common)
add_subdirectory(std)

add_executable(demo ${DIR_SRCS})

# install package first:
# yum install e2fsprogs-devel
# yum install uuid-devel
# yum install libuuid-devel

#在Linux平台上，依赖其他库的库一定要放到被依赖库的前面，否则链接时会出现找不到符号定义（undefined reference to）的错误
target_link_libraries(demo std common uuid)

#MAC平台上，可以调换依赖库和被依赖库顺序
#target_link_libraries(demo common std uuid)
