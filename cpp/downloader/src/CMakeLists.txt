cmake_minimum_required (VERSION 2.8)

project(downloader)

add_definitions(-std=c++11)

include_directories(../include)
#include_directories(../third_party/leveldb/include)

aux_source_directory(. DIR_SRCS)

#add_library(libleveldb STATIC IMPORTED)
#set_property(TARGET libleveldb PROPERTY IMPORTED_LOCATION ../third_party/leveldb/build/libleveldb.a)

add_executable(downloader ${DIR_SRCS})

#在Linux平台上，依赖其他库的库一定要放到被依赖库的前面，否则链接时会出现找不到符号定义（undefined reference to）的错误
target_link_libraries(downloader pthread curl)

#MAC平台上，可以调换依赖库和被依赖库顺序
#target_link_libraries(downloader common std uuid)
