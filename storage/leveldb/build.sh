#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
srcpath="$basepath/src"
buildpath="$basepath/build"
thirdparty="$basepath/third_party"
leveldb="$basepath/third_party/leveldb"
BINNAME="leveldbdemo"

param1=$1

# build third-party library first
if [ ! -d $leveldb ];then
    mkdir -p $thirdparty
    cd $thirdparty
    git clone --recurse-submodules https://github.com/google/leveldb.git
    
    cd $leveldb
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

    if [];then
        echo "[ERROR] Build leveldb library failed..."
    fi
    
    cd $basepath
fi

# ./build.sh    --clean, make and run
# ./build.sh run   --just run
# ./build.sh clean   --just clean build directory
if [ -z "$param1" ]; then
    rm -rf "$buildpath"
    mkdir "$buildpath" > /dev/null
    # -S and -B does not work in cmake lower version, such as 3.10.2
    # cmake -S "$srcpath" -B "$buildpath"
    cd "$buildpath"
    cmake "$srcpath"
    make -C "$buildpath"
    time "$buildpath/$BINNAME"
    cd ..
elif [ "$param1" = "run" ]; then
    time "$buildpath/$BINNAME"
elif [ "$param1" == "clean" ]; then
    rm -r "$buildpath"
fi