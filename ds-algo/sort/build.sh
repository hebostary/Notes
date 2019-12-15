#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
srcpath="$basepath/src"
buildpath="$basepath/build"

param1=$1

# ./build.sh    --clean, make and run
# ./build.sh    --just run
if [ -z "$param1" ];then
    rm -rf "$buildpath"
    mkdir "$buildpath" > /dev/null
    cd "$buildpath"
    cmake "$srcpath"
    make
elif [ "$param1" == "run" ];then
    cd "$buildpath"
fi

time ./demo

cd ..
