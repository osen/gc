#!/bin/sh

mkdir -p bin
#gcc -g -Wall -o bin/test -Iinclude src/*.c
gcc -O3 -Wall -o bin/test -Iinclude src/*.c
