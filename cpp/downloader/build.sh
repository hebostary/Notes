#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
srcpath="$basepath/src"
buildpath="$basepath/build"
thirdparty="$basepath/third_party"
BINNAME="downloader"

param1=$1

# ./build.sh    --clean, make and run
# ./build.sh run   --just run
# ./build.sh clean   --just clean build directory
if [ -z "$param1" ]; then
    rm -rf "$buildpath"
    mkdir "$buildpath" > /dev/null
    cmake -S "$srcpath" -B "$buildpath"
    make -C "$buildpath"
    time "$buildpath/$BINNAME"
    cd ..
elif [ "$param1" = "run" ]; then
    time "$buildpath/$BINNAME"
elif [ "$param1" == "clean" ]; then
    rm -r "$buildpath"
fi