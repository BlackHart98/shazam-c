#!/bin/bash

set -e

TARGET_NAME=main

if [ -d "bin" ]; then
    rm -rf ./bin
fi

mkdir ./bin

clang main.c -o ./bin/$TARGET_NAME

echo "Compiled successfully."