#!/bin/bash

param1=$1

basepath=$(cd `dirname $0`; pwd)
srcpath="$basepath/src"
buildpath="$basepath/build"
baseos="redhat"

which apt-get > /dev/null
if [ $? -eq 0 ];then
  baseos="ubuntu"
fi


if [ ! -f "/usr/include/uuid/uuid.h" ];then
    if [ "$baseos" == "redhat" ];then
        yum install e2fsprogs-devel -y
        yum install uuid-devel -y
        yum install libuuid-devel -y
    else
        sudo apt-get update
        sudo apt-get install uuid-dev
        sudo apt-get install e2fslibs-dev
    fi
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