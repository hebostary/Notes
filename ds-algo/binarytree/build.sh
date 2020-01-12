#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
srcpath="$basepath/src"
buildpath="$basepath/build"

param1=$1

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