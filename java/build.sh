#!/bin/bash

project=$1

function usage()
{
    echo "usage:"
    echo "  sh build.sh corejava"
}

function run()
{
    if [ "$project" == "corejava" ];then
        # clean
        rm -rf ./bin/* ./pkg/*

        # compile
        javac -d ./bin -cp ./src:$CLASSPATH ./src/CoreJavaTest.java ./src/com/hebostary/corejava/*.java
    
        #package
        cd ./bin
        jar -cvfe  ../pkg/CoreJavaTest.jar CoreJavaTest .
    
        # run
        cd -
        java -jar pkg/CoreJavaTest.jar
    fi
}

# main
if [ "$project" == "corejava" ];then
    echo "[INFO] Compiling and running $project"
    run
else
    usage
    exit
fi

