#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
srcpath="$basepath/src"
buildpath="$basepath/build"

rm -rf "$buildpath"

mkdir "$buildpath" > /dev/null

cd "$buildpath"

cmake "$srcpath"

make

./demo

cd ..
