#! /bin/sh
export $(cat .env)

DYLD_LIBRARY_PATH=./libs/ build/test