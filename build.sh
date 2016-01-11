#!/bin/sh

mkdir -p bin
#gcc -g -Wall -o bin/test -Iinclude src/gc.c src/main.c
gcc -O3 -Wall -o bin/test -Iinclude src/gc.c src/main.c
gcc -g -Wall -o bin/lists -Iinclude src/gc.c src/lists.c
