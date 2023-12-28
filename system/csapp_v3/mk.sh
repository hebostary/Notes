#!/bin/bash

e_cha=$1
e_num=$2
e_file="e_${e_cha}.${e_num}"
cd "c${e_cha}"

if [ -f "${e_file}.cpp" ];then
    echo "[Info] Begin to compile <${e_file}.cpp>"
    g++ ${e_file}.cpp -o $e_file
    if [ $? -eq 0 ];then
        echo "[Info] Begin to run <$e_file>"
        echo "=============================="
        ./$e_file
        echo "=============================="
        rm -f $e_file
    else
        echo "[Error] Compile <${e_file}.cpp> failed"
    fi
else
    echo "[Error] The <${e_file}> not existing"
fi

cd -