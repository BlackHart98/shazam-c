#!/bin/bash

set -e

TARGET_NAME=main

if [ -d "build" ]; then
    rm -rf ./build
fi

mkdir ./build

clang -fsanitize=address -g -O0 ./src/main.c -o ./build/$TARGET_NAME

echo "Compiled successfully."