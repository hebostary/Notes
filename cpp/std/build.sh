#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
srcpath="$basepath/src"
buildpath="$basepath/build"

param1=$1

rpm -qa |grep libuuid-devel
if [ $? -ne 0 ];then
  yum install e2fsprogs-devel -y
  yum install uuid-devel -y
  yum install libuuid-devel -y
fi

# for vs code build & debug tasks
mkdir -p buildpath="$basepath/bin"

# ./build.sh    --clean, make and run
# ./build.sh run   --just run
# ./build.sh clean   --just clean build directory
if [ -z "$param1" ];then
    rm -rf "$buildpath"
    mkdir "$buildpath" > /dev/null
    cd "$buildpath"
    cmake "$srcpath"
    make
    time ./demo
    cd ..
elif [ "$param1" == "run" ];then
    time ./"$buildpath"/demo
elif [ "$param1" == "clean" ];then
    rm -r "$buildpath"
fi